/*---------------------------------------------------------*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
/*---------------------------------------------------------*/
#include "memory.h"
/*---------------------------------------------------------*/
int minit(Ms  *ms)
{
    assert(NULL != ms);
    char c;
    int flag = (IPC_CREAT | 0666);

    if ((ms->shmid = shmget(ms->key, ms->sz, IPC_CREAT | 0666)) < 0) {
        perror("failed shmget");
        return 1;
    }

    if ((ms->shm = shmat(ms->shmid, NULL, 0)) == (char *) - 1) {
        perror("shmat");
        return 2;
    }

    //printf("attached segment from %p to %p with len %zu\n", ms->shm, (char *)ms->shm + ms->sz, (unsigned long)ms->sz);
    return 0;
}
/*---------------------------------------------------------*/
static void testWrite(char *s)
{
    char c;
    char *done = s - 1;

    if ('*' == *done) {
        printf("already wrote\n");
        return;
    }

    else printf("writing now\n");

    for (c = 'a'; c <= 'z'; c++)
        *s++ = c;

    s = 0;
    *done = '*';
}
/*---------------------------------------------------------*/
static void testRead(char *s)
{
    char *done = s - 1;

    if ('*' == *done)
        printf("str is <%s>\n", s);
}
/*---------------------------------------------------------*/
static void testBigWrite(char *s)
{
    long long i = 0;
    for (i = 0; i < (100 * 1024 * 1024) - 1; i++) {
        *s = '\0';
        s++;
    }
}
/*---------------------------------------------------------*/
//#define TEST_MEMORY
#ifdef TEST_MEMORY
static int test_main(void)
{
    char c;
    char *s, *t;
    Ms ms;
    ms.key = 5678;
    ms.sz = 100 * 1024 * 1024;
    ms.pattern = 0xDEADBEEF;
    assert(0 == minit(&ms));
    s = ms.shm;
    s++;
    testWrite(s);
    testRead(s);
    testBigWrite(s);
    exit(0);
}
#endif //TEST_MEMORY
/*---------------------------------------------------------*/
