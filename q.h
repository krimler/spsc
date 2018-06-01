#pragma once
/*---------------------------------------------------------*/
typedef struct Elem {
    int sz;
    char *line;
} Elem;

typedef struct queue {
    int max_size;
    int curr_size;
    int head;
    int tail;
    struct Elem **e;
} queue;
/*---------------------------------------------------------*/
queue *q_init(int max_size);
/*---------------------------------------------------------*/
bool q_add(queue *q, char *line, int sz);
/*---------------------------------------------------------*/
Elem *q_remove(queue *q);
/*---------------------------------------------------------*/
