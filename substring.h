#pragma once

typedef struct substring {
    int count;
    int *locations;
    int max_occurance;
    char *pattern;
    int state;
    int total_states;
    long long int total;
    bool done;
} sstring;

sstring *sstring_init(char *substring);

/* can be called multiple times on the incoming data stream. */
void sstring_compute(sstring *ss, char *data, int len);
void sstring_destory(sstring *ss);
void print_sstring(sstring *ss);

