/*------------------------------------------------------------------*/
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include<pthread.h>
/*------------------------------------------------------------------*/
#include "common.h"
/*------------------------------------------------------------------*/
uint32_t LOG_PRIORITY = INFO;
//https://gist.github.com/suyash/2488ff6996c98a8ee3a84fe3198a6f85
static int sock;
static int doack;
static volatile int doack_mutex;

static char *input_file;
static FILE *input_file_ptr;
static volatile int input_file_lock;
#ifdef threaded
static queue *q;
static pthread_t *thread_t;
//static pthread_mutex_t qlock =  PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
static volatile int qlock;
#define NUM_THREADS 1
#define PARALLEL_BUFFERS 1
#endif
/*------------------------------------------------------------------*/
#ifdef threaded
static void keep_buffers_ready(void *threadid)
{
    char *t;
    bool finished_reading;
    int llen;
    while (1) {
        lock(&qlock);
        if (q_full(q)) {
            unlock(&qlock);
            sleep(1);
            continue;
        }
        shm_buffer  = shm_alloc(jane_config->use_buffers);
        if (NULL == shm_buffer) {
            /* should not happen. */
            assert(0);
        }
        finished_reading = read_input_file(shm_buffer, &llen);
        if (finished_reading) {
            return;
        }
        assert(false != q_add(q, shm_buffer, llen));
        unlock(&qlock);
    }
}
/*------------------------------------------------------------------*/
static char *get_ready_buffer()
{
    Elem *e;
    char *s;
    while (1) {
        lock(&qlock);
        if (q_empty(q)) {
            unlock(&qlock);
            sleep(1);
            continue;
        }
        e = q_remove(q);

        unlock(&qlock);
        return e;
    }
}
/*------------------------------------------------------------------*/
static void free_buffers(char *shm_buffer)
{
    lock(&qlock);
    shm_dealloc((jane_config->use_buffers), shm_buffer);
    unlock(&qlock);
}
#endif
/*------------------------------------------------------------------*/
inline static bool setAck(bool value)
{
    lock(&doack_mutex);
    bool prev = doack;
    doack = value;
    unlock(&doack_mutex);
    return prev;
}
/*------------------------------------------------------------------*/
inline static bool isAck(void)
{
    return doack;
}
/*------------------------------------------------------------------*/
void set_input_file(const char *ip_file)
{
    assert(NULL != ip_file);
    lock(&input_file_lock);
    if (NULL != input_file) {
        Free(input_file);
        input_file = NULL;
    }
    input_file = strdup(ip_file);
    unlock(&input_file_lock);
}
/*------------------------------------------------------------------*/
//buffer is guaranteed to be of size use_buffers*BIG_ENOUGH.a
// buffer has to be allocated and free by callee.
//returns false if read has finished.
bool threaded_read_input_file(char *buffer, int *len)
{
    lock(&input_file_lock);
    assert(NULL != input_file);
    size_t newLen;
    if (NULL == input_file_ptr) {
        input_file_ptr = fopen(input_file, "r");
    }
    assert(NULL != input_file_ptr);
    newLen = fread(buffer, sizeof(char), ((jane_config->use_buffers) * BIG_ENOUGH) - 1, input_file_ptr);
    if (newLen == 0) {
        //stop reading.
        unlock(&input_file_lock);
        return false;
    }
    *len = (int)newLen;
    buffer[newLen++] = '\0'; /* Just to be safe. */
    LOGD("<%s>", buffer);
    unlock(&input_file_lock);
    return true;
}
/*------------------------------------------------------------------*/
//buffer is guaranteed to be of size use_buffers*BIG_ENOUGH.a
// buffer has to be allocated and free by callee.
//returns false if read has finished.
bool read_input_file(char *buffer, int *len)
{
    lock(&input_file_lock);
    assert(NULL != input_file);
    size_t newLen;
    if (NULL == input_file_ptr) {
        input_file_ptr = fopen(input_file, "r");
    }
    assert(NULL != input_file_ptr);
    newLen = fread(buffer, sizeof(char), ((jane_config->use_buffers) * BIG_ENOUGH) - 1, input_file_ptr);
    if (newLen == 0) {
        //stop reading.
        unlock(&input_file_lock);
        return false;
    }
    *len = (int)newLen;
    buffer[newLen++] = '\0'; /* Just to be safe. */
    LOGD("<%s>", buffer);
    unlock(&input_file_lock);
    return true;
}
/*------------------------------------------------------------------*/
int client_socket_init()
{
    for (;;) {
        const char *server_name = "localhost";
        const int server_port = jane_config->port;

        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;

        // creates binary representation of server name
        // and stores it as sin_addr
        // http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
        inet_pton(AF_INET, server_name, &server_address.sin_addr);

        // htons: port in network order format
        server_address.sin_port = htons(server_port);

        // open a stream socket
        if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            LOGD("could not create socket\n");
            return 1;
        }

        // TCP is connection oriented, a reliable connection
        // **must** be established before any data is exchanged
        if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
            LOGI("Jane is failed to connect to Thor!, retrying...\n");
            sleep(1);
        }
        else break;
    }
    return 0;
}
/*------------------------------------------------------------------*/
int client_socket_send(int num, int offset, char *data_to_send, int dlen, bool ack)
{
    errno = 0;
    char buf[BIG_ENOUGH]  = {0};
    int i;
    char *str = NULL;
    size_t strSize = 0;
    FILE *stream = open_memstream(&str, &strSize);
    assert(NULL != stream);
    fprintf(stream, "%s", (ack ? START_WITH_ACK_MARKER : START_MARKER));
    int times = num;
    int j;
    fprintf(stream, "%d,%d,%d,%s%s", num, offset, dlen, data_to_send, END_MARKER);
    fclose(stream);
    send(sock, str, strSize, 0);
    free(str);
    return errno;
}
/*------------------------------------------------------------------*/
int client_socket_rcv(char **buffer)
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
        LOGD("got something\n");
        if (n <= 0) {
            LOGI("error during rcv");
            return 1;
        }
        pbuffer += n;
        maxlen -= n;
        len += n;
    }
    while (NULL == (start = strstr(*buffer, START_MARKER)) || NULL == (end = strstr(*buffer, END_MARKER)));
    pbuffer = *buffer;
    *end = '\0';
    char *t = start + strlen(START_MARKER);
    strcpy(pbuffer, t); //(*pbuffer++ == *t++);
    LOGD("after memmove <%s>\n", *buffer);
    return 0;
}
/*------------------------------------------------------------------*/
int client_socket_close()
{
    // close the socket
    close(sock);
    return 0;
}

/*------------------------------------------------------------------*/
static int client_loop()
{
    char *str[] = { "abc", "def", "ghi"};
    bool finished_reading = true;
    char *shm_buffer = NULL;
    char md5[BIG_ENOUGH];
    bool failed = false;
    char *got = NULL;
    Elem *e = NULL;
    int llen = 0;
    setAck(false);
    while (true) {
        int res;
START:
        res = client_socket_init();
        if (0 != res) continue;
        while (finished_reading) {
            if (NULL != shm_buffer) {
                //Free(shm_buffer);
#ifdef threaded
                free_buffers(jane_config->use_buffers, shm_buffer);
#else
                shm_dealloc((jane_config->use_buffers), shm_buffer);
#endif
                shm_buffer = NULL;
            }
#ifdef threaded
            e = get_ready_buffer();
            if (!failed && NULL != e) {
                shm_buffer = e->line;
                llen = e->sz;
            }

#else
            shm_buffer = shm_alloc(jane_config->use_buffers);
            assert(NULL != shm_buffer);
            if (!failed) {
                finished_reading = read_input_file(shm_buffer, &llen);

                if (!finished_reading) {
                    continue;
                }
#endif
            readable_hash(shm_buffer, llen, 1, md5, BIG_ENOUGH - 1);
        }
        if (0 != client_socket_send(1, get_offset(shm_buffer), md5, llen, isAck())) {
            failed = true;
            goto START;
        }

        if (0 != client_socket_rcv(&got)) {
            failed = true;
            goto START;
        }
        if (0 == strcmp(got, md5)) {
            LOGD("matched\n");
            failed = false;
            Free(got);
            got = NULL;
        }
        else {  /* Loki has changed message, re-try. */
            Free(got);
            got = NULL;
            continue;
        }

    }
    break;
}
LOGI("done------->\n");
send(sock, DONE_TRANS, sizeof(DONE_TRANS), 0);
return client_socket_close();

}
/*------------------------------------------------------------------*/
int main(void)
{
    /* Verify config read is success. */
    assert(true == asgard_config_read());
    /* Then Jane's config must be ready. */
    assert(NULL != jane_config);
    /* consume the available config. */
    set_input_file(jane_config->input_text_file);
    init_memory(jane_config->buffers);

    /* Tell the people of Asgard who I am. */
    asgard_init(M_JANE);
#ifdef threaded
    q = queue_init(PARALLEL_BUFFERS);
    int rc = pthread_create(&thread, NULL, keep_buffers_ready, (void *)t);
#endif
    /* Now as everyone knows who I am, wait for right opportunity. */
    client_loop();
    return 0;
}
/*------------------------------------------------------------------*/
