/*---------------------------------------------------------*/
#include <stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <string.h>
#include<assert.h>
/*---------------------------------------------------------*/
#include "q.h"
/*---------------------------------------------------------*/
static void add_counter(queue *q, int *a)
{
    (*a)++;
    if ((*a) >= q->max_size) {
        *a = 0;
    }
}
/*---------------------------------------------------------*/
/*
static void dec_counter(queue*q, int*a){
    (*a)--;
    if((*a) < 0)
       *a = q->max_size;
}
*/
/*---------------------------------------------------------*/
queue *q_init(int max_size)
{
    queue *t = calloc(1, sizeof(queue));
    t->e = calloc(max_size, sizeof(Elem));
    t->max_size = max_size;
    assert( 0 < max_size);
    return t;
}
/*---------------------------------------------------------*/
bool q_full(queue *q)
{
    if (q->curr_size == q->max_size)
        return true;
    return false;
}
/*---------------------------------------------------------*/
bool q_add(queue *q, char *str_elem, int sz)
{
    if (q->curr_size == q->max_size)
        return false;
    Elem *e = calloc(1, sizeof(Elem));
    e->line = str_elem;
    e->sz = sz;
    q->e[q->head] = e;
    add_counter(q, &(q->head));
    (q->curr_size)++;
    return true;
}
/*---------------------------------------------------------*/
bool q_empty(queue *q)
{
    if (q->curr_size == 0) {
        return true;
    }
    return false;
}
/*---------------------------------------------------------*/
Elem *q_remove(queue *q)
{
    if (q->curr_size == 0)
        return NULL;

    Elem *t = q->e[q->tail];
    add_counter(q, &(q->tail));
    (q->curr_size)--;

    return t;
}
/*---------------------------------------------------------*/
void print_q(queue *q)
{
    printf("q max_size is %d, and current_size is %d\n.", q->max_size, q->curr_size);
#define DEBUG_Q
#ifdef DEBUG_Q
    printf("q values are");
    int i;
    for (i = 0; i < q->curr_size; i++) {
        printf("value [%d]-><%s> of size %d\n", i, q->e[i]->line, q->e[i]->sz);
    }
#endif
}
/*---------------------------------------------------------*/
static int T1(void)
{
    queue *q = q_init(3);
    char *a = "madhav";
    char *b = "gaiwkad";
    char *c = "wow";
    assert(true == q_add(q, a, 5));
    assert(true == q_add(q, b, 10));
    assert(true == q_add(q, c, 15));
    Elem *be = q_remove(q);
    assert("madhav" == be->line);
    char *d = "malhar";
    char *e  = "bhau";
    assert(true == q_add(q, d, 20));
    assert(false == q_add(q, e, 25));
    print_q(q);
    be = q_remove(q);
    assert("gaiwkad" == be->line);
    be = q_remove(q);
    assert("wow" == be->line);
    be = q_remove(q);
    assert("malhar" == be->line);
    be = q_remove(q);
    assert(NULL == be);
}
/*------------------------------------------------------------------*/
static int T2(void)
{
    queue *q = q_init(1);
    char *a = "madhav";
    char *b = "gaiwkad";
    char *c = "wow";
    assert(true == q_add(q, a, 5));
    assert(false == q_add(q, b, 10));
    assert(false == q_add(q, c, 15));
    Elem *be = q_remove(q);
    assert("madhav" == be->line);
    char *d = "malhar";
    char *e  = "bhau";
    assert(true == q_add(q, d, 20));
    assert(false == q_add(q, e, 25));
    print_q(q);
    be = q_remove(q);
    assert("malhar" == be->line);
    be = q_remove(q);
    assert(NULL == be);
}
/*------------------------------------------------------------------*/
#ifdef DEBUG_Q
int main(void)
{
    T1();
    T2();
    return 0;
}
#endif
/*---------------------------------------------------------*/
