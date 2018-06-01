/*---------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
/*---------------------------------------------------------*/
#include "substring.h"
/*---------------------------------------------------------*/
#define MAX_OCCURANCE 1024
/*---------------------------------------------------------*/
void print_sstring(sstring *ss)
{
    printf("--------> pattern stastics <---------------\n");
    printf("pattern <%s> is found %d times.\n", ss->pattern, ss->count);
    int i;
    for (i = 0; i < ss->count; i++) {
        printf("lcoation[%d] = %d.\n", i, ss->locations[i]);
    }
//#define DEBUG_SSTRING
#ifdef DEBUG_SSTRING
    printf("max occurance allowed is %d.\n", ss->max_occurance);
    printf("current state = %d, total_states are  = %d done = %d.\n", ss->state, ss->total_states, ss->done);
#endif
    printf("-------------------------------------------\n");
}
/*---------------------------------------------------------*/
sstring *sstring_init(char *substring)
{
    sstring *s = calloc(1, sizeof(sstring));
    s->pattern = strdup(substring);
    s->locations = calloc(MAX_OCCURANCE, sizeof(int));
    s->max_occurance = MAX_OCCURANCE;
    s->total_states = strlen(substring);
    return s;
}
/*---------------------------------------------------------*/
void sstring_compute(sstring *ss, char *data, int len)
{
    int i;
    for (i = 0; i < len && !(ss->done); i++, (ss->total)++) {
        if (data[i] == ss->pattern[ss->state]) {
            ss->state++;
            if (ss->state == ss->total_states) {
                ss->state = 0;
                ss->locations[ss->count] = ss->total - (ss->total_states - 1);
                (ss->count)++;
                if (ss->max_occurance == ss->count) {
                    ss->done = true;
                }
            }
        }
        else {
            ss->state = 0;
        }
    }
}
/*---------------------------------------------------------*/
void sstring_destory(sstring *ss)
{
    free(ss->pattern);
    ss->pattern = NULL;
    free(ss->locations);
    ss->locations = NULL;
}
/*---------------------------------------------------------*/
#ifdef DEBUG_SSTRING
int main(void)
{

    sstring *ss = sstring_init("mad");
    char *d = "madgaimad";
    sstring_compute(ss, d, strlen(d));
    sstring_compute(ss, d, strlen(d));
    print_sstring(ss);
    sstring_destory(ss);
    return 0;
}
#endif
/*---------------------------------------------------------*/
