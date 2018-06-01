/*------------------------------------------------------------------*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
/*------------------------------------------------------------------*/
#include "common.h"
/*------------------------------------------------------------------*/
uint32_t LOG_PRIORITY = INFO;
/**
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * */
static int sock;
int listen_sock;

static FILE *thor_output_file;
static volatile int thor_output_file_lock;
static const char *thor_output_file_str;

static sstring *substring_context;
/*------------------------------------------------------------------*/
static void server_flush_transaction(const char *data, int len)
{
    lock(&thor_output_file_lock);
    if (NULL == thor_output_file) {
        thor_output_file = fopen(thor_output_file_str, "a");
    }
    assert(NULL != thor_output_file);
    fwrite((void *)data, 1, len, thor_output_file);
    fflush(thor_output_file);
    unlock(&thor_output_file_lock);
}
/*------------------------------------------------------------------*/
static void server_truncate_output_file()
{
    lock(&thor_output_file_lock);
    if (NULL == thor_output_file) {
        unlock(&thor_output_file_lock);;
        return;
    }
    fclose(thor_output_file);
    thor_output_file = fopen(thor_output_file_str, "w");
    fclose(thor_output_file);
    thor_output_file =
        NULL; /* as we have marked it NULL, it will be opend in append mode next time via server_flush_transaction() when called. */
    unlock(&thor_output_file_lock);
}
/*------------------------------------------------------------------*/
int server_socket_init()
{
    // port to start the server on
    int SERVER_PORT = jane_config->port;

    // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    // htons: host to network short: transforms a value in host byte
    // ordering format to a short value in network byte ordering format
    server_address.sin_port = htons(SERVER_PORT);

    // htonl: host to network long: same as htons but to long
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // create a TCP socket, creation returns -1 on failure
    if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        LOGI("could not create listen socket\n");
        return 1;
    }
    /* on my mac bind does takes lot more time, workaround is to set reuse port. */
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEPORT, &(int) {
    1
}, sizeof(int)) < 0)
    LOGD("setsockopt(SO_REUSEPORT) failed");

    // bind it to listen to the incoming connections on the created server
    // address, will return -1 on error
    if ((bind(listen_sock, (struct sockaddr *)&server_address,
              sizeof(server_address))) < 0) {
        LOGD("could not bind socket");
        return 1;
    }

    int wait_size = 16;  // maximum number of waiting clients, after which
    // dropping begins
    if (listen(listen_sock, wait_size) < 0) {
        LOGD("could not open socket for listening");
        return 1;
    }
    return 0;
}
/*------------------------------------------------------------------*/
int server_accept()
{
    // socket address used to store client address
    struct sockaddr_in client_address;
    int client_address_len = 0;

    // open a new socket to transmit data per connection
    if ((sock =
             accept(listen_sock, (struct sockaddr *)&client_address,
                    (socklen_t *)&client_address_len)) < 0) {
        LOGD("could not open a socket to accept data");
        return 1;
    }
    //server_truncate_output_file();
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
/* accept input string, fill md5, num and offset. */
bool parse_data(char *input, int *num, int *offset, int *len, char *md5)
{
    char *t = input;
    assert(NULL != input);
    char *comma = strstr(input, ",");
    assert(NULL != comma);
    char nums[24] = {0};
    char *pt = nums;
    while (t != comma) {
        *pt = *t;
        pt++;
        t++;
    }
    *pt = 0;

    *num = atoi(nums);
    assert(*num > 0);
    comma++;
    t = comma;

    char *comma1 = strstr(comma, ",");
    pt = nums;
    while (t != comma1) {
        *pt = *t;
        pt++;
        t++;;
    }
    *pt = 0;

    *offset = atoi(nums);
    assert(*offset > 0);
    comma1++;
    t = comma1;
    comma = comma1;
    comma1 = strstr(comma, ",");
    pt = nums;
    while (t != comma1) {
        *pt = *t;
        pt++;
        t++;;
    }
    *pt = 0;
    *len = atoi(nums);
    assert(*len >= 0);

    comma1++;
    //t++;
    strcpy(md5, comma1);
    assert('\0' != md5[0]);
    return true;
}
/*------------------------------------------------------------------*/
void pr_data(char *m, int num, int len)
{
    char a[1024] = {};

    snprintf(a, len, "%s", m);
}
/*------------------------------------------------------------------*/
int server_socket_send(char **data_to_send)
{
    int offset;
    int num;
    int len;
    int newlen;
    char md5 [1024] = {};

    parse_data(*data_to_send, &num, &offset, &len, md5);
    char  *data = get_buffer(offset);
    //pr_data(data, num, len);
    errno = 0;
    char buf[BIG_ENOUGH]  = {0};

    bool intact = does_string_intact(data, len, num, md5);
    if (intact) {
        newlen = snprintf(buf, BIG_ENOUGH, "%s%s%s", START_MARKER, *data_to_send, END_MARKER);
        server_flush_transaction(data, len);
        if (thor_config->do_search) {
            sstring_compute(substring_context, data, len);
        }
        send(sock, buf, newlen, 0);
    }
    else {
        newlen = snprintf(buf, BIG_ENOUGH, "%s%s%s", START_MARKER, "--------->loki has changed message.<------", END_MARKER);
        send(sock, buf, newlen, 0);
        printf("not intact\n");
    }
    return errno;
}
/*------------------------------------------------------------------*/
int server_socket_rcv(char **buffer)
{
    int n = 0;
    int len = 0, maxlen = BIG_ENOUGH;
    char *start;
    char *end;
    *buffer = Calloc(1, maxlen);
    char *pbuffer = *buffer;
    // will remain open until the server terminates the connection
    do {
        n = recv(sock, pbuffer, maxlen, 0);
        LOGD("got something");
        if (n <= 0) {
            return 1;
        }
        if (NULL != strstr(*buffer, DONE_TRANS))
            return 1;
        pbuffer += n;
        maxlen -= n;
        len += n;
    }
    while (NULL == (start = strstr(*buffer, START_MARKER)) || NULL == (end = strstr(*buffer, END_MARKER)));
    pbuffer = *buffer;
    *end = '\0';
    char *t = start + strlen(START_MARKER);
    strcpy(pbuffer, t);
    return 0;
}
/*------------------------------------------------------------------*/
int server_socket_close()
{
    // close the socket
    close(sock);
    close(listen_sock);
    return 0;
}
/*------------------------------------------------------------------*/
int server_loop()
{
    static char *prev;
    static char *curr;
    char *buf = NULL;
    int res;
    LOGD("do_server_init");
    res = server_socket_init();
    while (true) {
START:
        LOGI("try to accept connection");
        res = server_accept();
        LOGD("accepted");

        if (0 != res) {
            /*failed to accept, retry. */
            LOGD("failed to accept");
            sleep(1);
            goto START;
        }
        else {
            while (true) {
                if (NULL != buf) {
                    Free(buf);
                    buf = NULL;
                }
                if (0 != server_socket_rcv(&buf)) {
                    /* failed to rcv, return to accept. */
                    if (thor_config->do_search) {
                        print_sstring(substring_context);
                    }
                    goto START;
                }
                LOGD("got from clie: <%s>", buf);

                if (0 != server_socket_send(&buf)) {
                    /* failed to send, return to accept. */
                    if (thor_config->do_search) {
                        print_sstring(substring_context);
                    }
                    goto START;
                }
            }
        }
    }
    return server_socket_close();

}
/*------------------------------------------------------------------*/
int main(void)
{
    /* Verify config read is success. */
    assert(true == asgard_config_read());
    /* Then thor's config must be ready. */
    assert(NULL != thor_config);
    /* consume the available config. */
    thor_output_file_str = thor_config->output_text_file;
    init_memory(thor_config->buffers);
    /* Tell the people of Asgard who I am. */
    asgard_init(M_THOR);

    if (thor_config->do_search) {
        substring_context = sstring_init(thor_config->pattern);
        assert(NULL != substring_context);
    }
    /* Now as everyone knows who I am, wait for right opportunity. */
    server_loop();
    return 0;
}
/*------------------------------------------------------------------*/
