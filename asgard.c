/*---------------------------------------------------------*/
#include "common.h"
#include "radix_sort.h"
/*---------------------------------------------------------*/
static Asgard asgard;
static Ms *memorys;
//#define RESTRICT_MEMBERS
static int members;
static int *free_pool;
static int *busy_pool;
/*---------------------------------------------------------*/
int get_offset(char *mem)
{
    return (mem - asgard.data_ptr);
}
/*---------------------------------------------------------*/
char *get_buffer(int offset)
{
    return (asgard.data_ptr + offset);
}
/*---------------------------------------------------------*/
static void free_pool_init(void)
{
    free_pool = Calloc(members, sizeof(int)); // global, should not be freed.
    busy_pool = Calloc(members, sizeof(int)); // global, should not be free.

    int i;
    for (i = 0; i < members; i++) {
        free_pool[i] = i + 1;
    }
}
/*---------------------------------------------------------*/
char *shm_alloc(int num)
{
    int i;
    assert(num <= members);
    char *root;
    //validate that we have so many buffers in free pools.
    for (i = 0; i < num; i++) {
        if (free_pool[i] == 0) {
            return NULL;
        }
    }
    //move buffers to busy pool, and set those elements to zero in free pool..
    root = asgard.data_ptr + (free_pool[0] * 1024);
    for (i = 0; i < num; i++) {
        busy_pool[members - 1 - i] = free_pool[i];
        free_pool[i] = 0;
    }
    /* move zero elements from free_pool from start to end. */
    move_array(free_pool, members);

    /* now sort the busy_pool */
    radix_sort(busy_pool, members);
    /* reverse it, so all elements go to end, including zero.. */
    move_array(busy_pool, members);

    return root;
}
/*---------------------------------------------------------*/
void shm_dealloc(int num, void *mem)
{
    /* get the starting element number. */
    int n = ((char *)mem - asgard.data_ptr) / 1024;
    int i = 0;
    int nn = 0;
    int binres = binarySearch(busy_pool, 0, members, n);
    assert(-1 != binres);

    /* remove elements from busy pool, by setting them to zero. */
    for (i = binres; i < num; i++) {
//for(i=0; i<num; i++)
        free_pool[members - 1 - i] = busy_pool[i];
        busy_pool[i] = 0;
    }
    /* now sort the free_pool */
    radix_sort(free_pool, members);
    /* reverse it, so all elements go to end, including zero.. */
    move_array(free_pool, members);

    /* move zero elements from busy_pool from start to end. */
    move_array(busy_pool, members);
}
/*---------------------------------------------------------*/
int buffer_init()
{
    assert(NULL == asgard.buffers);
    members = (asgard.ms.sz / 1024) - 1;
#ifdef RESTRICT_MEMBERS
    members = 3;
#endif
    free_pool_init();

    asgard.buffers = (Buffer *)calloc(members, sizeof(Buffer));
    assert(NULL != asgard.buffers);
    long long int i;
    char *temp = asgard.data_ptr;
    for (i = 0; i < members; i++) {
        asgard.buffers[i].array = temp + (i * 1024);
        asgard.buffers[i].busy = false;
    }
}
/*---------------------------------------------------------*/
/*
char  *shm_alloc()
{
    static Buffer *last;
    if (NULL == last) {
        last  = &(asgard.buffers[0]);
    }
    while (last->busy) {
        last++;
    }
    last->busy = true;
    return last->array;
}

int shm_dealloc(void *mem)
{
    Buffer *b = (Buffer *)mem;
    assert(true == b->busy);
    b->busy = false;
    memset((b), 0, 1024);
};
*/
/*---------------------------------------------------------*/
int asgard_init(MRole role)
{
    char c;
    char *s, *t;
    Ms *ms = memorys;
    assert(0 == minit(ms));
    asgard.data_ptr = ms->shm;
    asgard.ms = *ms;
    if (role == M_JANE) {
        buffer_init();
    }
}
/*---------------------------------------------------------*/
void init_memory(int buffers)
{
    memorys = Calloc(1, sizeof(Ms)); /* will not be freed */
    assert(NULL != memorys);
    memorys->key = 5678;
    memorys->sz = (buffers * 1024 ) + 1024 ;
    memorys->pattern = 0xDEADBEEF;
}
/*---------------------------------------------------------*/
