#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
/*--------------------------------------------------------*/
#include "util.h"
/*--------------------------------------------------------*/
volatile int ts_lock; /* public variable used to lock staic timestamp buffer in TimeStamp(). */
/*--------------------------------------------------------*/
char *
TimeStamp(void)
{
#define MAXTIMELEN 128
    static char timestamp[MAXTIMELEN];
    long usec;
    struct timeval curTime;
    char buffer [MAXTIMELEN];

    gettimeofday(&curTime, NULL);
    usec = curTime.tv_usec;
    strftime(buffer, MAXTIMELEN, "%Y-%m-%d_%H-%M-%S", localtime(&curTime.tv_sec));
    snprintf(timestamp, MAXTIMELEN, "%s_%ld", buffer, usec);
    return (timestamp);
}
/*--------------------------------------------------------*/
static char table(uint8_t num)
{
    switch (num) {
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';
        default:
            return '0';
    }
    return '0'; //I am a dead statement.
}
/*--------------------------------------------------------*/
static uint8_t nibbles(char ch)
{
    switch (ch) {
        case '1':
            return 0x1;
        case '2':
            return 0x2;
        case '3':
            return 0x3;
        case '4':
            return 0x4;
        case '5':
            return 0x5;
        case '6':
            return 0x6;
        case '7':
            return 0x7;
        case '8':
            return 0x8;
        case '9':
            return 0x9;
        case 'A':
            return 0xA;
        case 'B':
            return 0xB;
        case 'C':
            return 0xC;
        case 'D':
            return 0xD;
        case 'E':
            return 0xE;
        case 'F':
            return 0xF;
        default:
            return 0x0;
    }
    return 0x0; // I am a dead statement.
}
/*--------------------------------------------------------*/
//https://stackoverflow.com/questions/23131420/extracting-nibbles-from-bytes-in-a-for-loop
//https://stackoverflow.com/questions/10324611/how-to-calculate-the-md5-hash-of-a-large-file-in-c
static void toDec(uint8_t *b, int blen, char *r, int rlen)
{
    int i;
    int j = 0;
    for (i = 0; i < blen; i++) {
        uint8_t nibble;
        nibble = (uint8_t) ((b[i] >> 4) & 0x0F);
        assert(rlen > j);
        r[j++] = table(nibble);
        nibble = (uint8_t) ((b[i] & 0x0F));
        assert(rlen > j);
        r[j++] = table(nibble);
    }
    r[j] = 0;
    assert(rlen > j);
}
/*--------------------------------------------------------*/
static void toBin(char *r, int rlen, char *b, int blen)
{
    int i;
    int j = 0;
    for (i = 0; i < rlen; i += 2) {
        assert(blen > j);
        b[j++] = (nibbles(r[i]) << 4 & 0xF0) & (nibbles(r[i + 1]) & 0x0F);
    }
}
/*--------------------------------------------------------*/
/* s is source string, d is its readable hash. */
void readable_hash(char *s, int slen, int snum, char *d, int dlen)
{
    char md5 [1024] = {0};
    assert (NULL != s);
    MD5_CTX mdContext;
    MD5_Init (&mdContext);
    MD5_Update (&mdContext, s, slen);
    MD5_Final ((unsigned char *)md5, &mdContext);
    toDec((uint8_t *)md5, MD5_DIGEST_LENGTH, d, dlen);
}
/*--------------------------------------------------------*/
/* s is source string, d is its readable hash, returns bool if d and hash(s) matches. */
bool does_string_intact(char *s, int slen, int num, char *d)
{
    char sd[BIG_ENOUGH];
    readable_hash(s, slen, num, sd, BIG_ENOUGH - 1);
    return (0 == strcmp(sd, d) ? true : false);
}
/*--------------------------------------------------------*/
static int test1()
{

    MD5_CTX mdContext;
    MD5_Init (&mdContext);
    char *b = "funisgun";
    int bl = strlen(b);
    char md5 [1024] = {0};

    MD5_Update (&mdContext, b, bl);
    MD5_Final ((unsigned char *)md5, &mdContext);

    char r [1024] = {0};
    int rlen = 1024;
    toDec((uint8_t *)md5, MD5_DIGEST_LENGTH, r, rlen);

    char b1[1024];
    int b1len = 1024;
    toBin(r, rlen, b1, b1len);
    char r1 [1024] = {0};
    int r1len = 1024;
    toDec((uint8_t *)md5, MD5_DIGEST_LENGTH, r1, r1len);

    return 0;
}
/*--------------------------------------------------------*/
/*intel platform depedent spinklock implementation. Upside is, it does not require pthread to build program. */
void lock(volatile int *exclusion)
{
    while (__sync_lock_test_and_set(exclusion, 1)) while (*exclusion);
}
/*--------------------------------------------------------*/
void unlock(volatile int *exclusion)
{
    __sync_lock_release(exclusion);
}
/*--------------------------------------------------------*/
static int test2()
{
    char *s = "funisgun";
    char d[1024] = {0};
    readable_hash(s, strlen(s), 1, d, 1024);
    int dint = (int)does_string_intact(s, strlen(s), 1,  d);
    printf("intact value is %d\n", dint);
    assert(1 ==  dint);
    return 0;
}
/*--------------------------------------------------------*/
static int test3()
{
    /* test empty string is hashable. */
    char *s = "";
    char d[1024] = {0};
    readable_hash(s, strlen(s), 1, d, 1024);
    printf("hash is %s\n", d);
    return 0;
}
/*--------------------------------------------------------*/
//#define TEST_HASH
#ifdef TEST_HASH
int main(void)
{

    assert(0 ==  test1());
    assert(0 == test2());
    assert( 0 == test3());
    return 0;
}
#endif //TEST_HASH
/*--------------------------------------------------------*/
