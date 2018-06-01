/*---------------------------------------------------*/
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <assert.h>
/*---------------------------------------------------*/
#include "ini_parser.h"
/*---------------------------------------------------*/
/* A very simple key value reader, simillar to ini-file parser but without "section" support.
 * Also comments can start only at start of line. */
/*---------------------------------------------------*/
/* how many max config lines will be supported. */
#define MAX_CONFIGS 10
#define CONFIG_FILE "config.ini"
#define INPUT_TEXT_FILE "INPUT_TEXT_FILE"
#define USE_BUFFERS "USE_BUFFERS"
#define BUFFERS "BUFFERS"
#define PATTERN "PATTERN"
#define OUTPUT_TEXT_FILE "OUTPUT_TEXT_FILE"
#define DO_SEARCH "DO_SEARCH"
#define JANE "PRODUCER"
#define THOR "CONSUMER"
/*---------------------------------------------------*/
/* global's are here folks. */
Jane_config *jane_config;
Thor_config *thor_config;
/*---------------------------------------------------*/
typedef struct ini_pair {
    char *key;
    char *value;
} ini_pair;
/*---------------------------------------------------*/
typedef struct section {
    ini_pair **config_pair;
    int count; /* maintains count of ini_paire elements. */
    char *name;
} section;
/*---------------------------------------------------*/
static bool read_line(char *input_file, char **k, char **v, char **section)
{
    *k = NULL;
    *v = NULL;
    assert(NULL != input_file);
    static FILE *input_file_ptr;
    size_t newLen = 0;
    char s[1024] = {0};
    if (NULL == input_file_ptr) {
        input_file_ptr = fopen(input_file, "r");
    }
    assert(NULL != input_file_ptr);
    char ch;
    int i = 0;
    while ((ch = fgetc(input_file_ptr))) {
        if (ch == '[') {
            /* advance to next char. */
            while ((ch = fgetc(input_file_ptr))) {
                if (ch == ']') {
                    *section = strdup(s);
                    return true;
                }
                if (ch == EOF) return false;
                s[i++] = ch;
            }
        }
        if (ch == EOF) {
            if (NULL == *k) return false;
            *v = strdup(s);
            return true;
        }
        if (ch == '#') {
            do {
                if (ch == '\n') break;
                if (ch == EOF) return false;
            }
            while ((ch = fgetc(input_file_ptr)));
            continue;
        }
        if (ch == '\t' || ch == ' ' || ch == '\r') continue;
        if (ch == '\n' && NULL == *k) continue;
        if (ch == '=') {
            *k = strdup(s);
            i = 0;
            memset(s, 0, 1024);
            continue;
        }
        if (ch == '\n') {
            *v = strdup(s);
            return true;
        }
        s[i++] = ch;
    }
    return false; /* dead statement. */
}
/*---------------------------------------------------*/
static ini_pair *get_empty_ini_pair(void)
{
    char *k = strdup("");
    char *v = strdup("");
    ini_pair *t = calloc(1, sizeof(ini_pair));
    t->key = k;
    t->value = v;
    return t;
}
/*---------------------------------------------------*/
static ini_pair *get_ini_pair(char *k, char *v)
{
    ini_pair *t = calloc(1, sizeof(ini_pair));
    t->key = k;
    t->value = v;
    return t;
}
/*---------------------------------------------------*/
static section *get_section(char *name)
{
    section *t = calloc(1, sizeof(section));
    assert(NULL != t);
    t->name = name;
    t->config_pair = calloc(MAX_CONFIGS, sizeof(ini_pair *));
    return t;
}
/*---------------------------------------------------*/
static void add_pair(section *s, char *k, char *v)
{
    assert(NULL != s && NULL != k && NULL != v);
    s->config_pair[s->count] = get_ini_pair(k, v);
    /* We dont support reading these many config sections. */
    if ((s->count)++ >= MAX_CONFIGS) assert(0);
}
/*---------------------------------------------------*/
static section **ini_parse(char *file_name, int *config_sections)
{
    section **sections = calloc(MAX_CONFIGS, sizeof(section));
    assert(NULL != sections);

    char *k;
    char *v;
    char *s;
    bool r = true;
    section *curr_section;
    while (r) {
        s = NULL;
        r = read_line(file_name, &k, &v, &s);
        //printf("k<%s>, v<%s>\n", k, v);
        if (r) {
            /* check if new section is available. */
            if (NULL != s) {
                curr_section = get_section(s);
                sections[*config_sections] = curr_section;
                /* We dont support reading these many config lines. */
                if ((*config_sections)++ >= MAX_CONFIGS) assert(0);
                continue;
            }
            add_pair(curr_section, k, v);
        }
    }
    return sections;
}
/*---------------------------------------------------*/
static void print_jane_config(void)
{
    printf("jane_config is:\n");
    printf("%s = %s\n", "input_text_file", jane_config->input_text_file);
    printf("%s = %d\n", "buffers", jane_config->buffers);
    printf("%s = %d\n", "use_buffers", jane_config->use_buffers);
    printf("%s = %d\n", "port", jane_config->port);
    printf("<-------->\n");
}
/*---------------------------------------------------*/
static void print_thor_config(void)
{
    printf("thor_config is:\n");
    printf("%s = %s\n", "output_text_file", thor_config->output_text_file);
    printf("%s = %d\n", "buffers", thor_config->buffers);
    printf("%s = %s\n", "pattern", thor_config->pattern);
    printf("%s = %d\n", "do_search", thor_config->do_search);
    printf("%s = %d\n", "port", thor_config->port);
    printf("<-------->\n");
}
/*---------------------------------------------------*/
bool asgard_config_read(void)
{
    int count = 0;
    section **config_sections = ini_parse(CONFIG_FILE, &count);
    assert(NULL != config_sections);
    int s;
    int i;
    int pair_count;
    ini_pair **pairs;
    if (NULL == jane_config) {
        jane_config = calloc(1, sizeof(Jane_config));
        assert(NULL != jane_config);
    }
    if (NULL == thor_config) {
        thor_config = calloc(1, sizeof(struct consumer_config));
        assert(NULL != thor_config);
    }
    for (s = 0; s < count ; s++) {

        if (0 == strcmp(JANE, config_sections[s]->name)) {
            //printf("section: %s\n", config_sections[s]->name);
            pairs = config_sections[s]->config_pair;
            pair_count = config_sections[s]->count;
            for (i = 0; i < pair_count; i++) {
                //printf("key: <%s>, value: <%s>\n", pairs[i]->key, pairs[i]->value);
                if (0 == jane_config->buffers && 0 == strcmp(BUFFERS, pairs[i]->key)) {
                    jane_config->buffers = atoi(pairs[i]->value);
                    if (jane_config->buffers < 2 || jane_config->buffers > 102399) {
                        //printf("program only supports 2 to 102399 buffers.\n");
                        exit(1);
                    }
                }
                if (NULL == jane_config->input_text_file && 0 == strcmp(INPUT_TEXT_FILE, pairs[i]->key)) {
                    jane_config->input_text_file = strdup(pairs[i]->value);
                }
                if (0 == jane_config->use_buffers && 0 == strcmp(USE_BUFFERS, pairs[i]->key)) {
                    jane_config->use_buffers = atoi(pairs[i]->value);
                }
                if (0 == jane_config->port && 0 == strcmp("port", pairs[i]->key)) {
                    jane_config->port = atoi(pairs[i]->value);
                }
            }
        }
        if (0 == strcmp(THOR, config_sections[s]->name)) {
            //printf("section: %s\n", config_sections[s]->name);
            pairs = config_sections[s]->config_pair;
            pair_count = config_sections[s]->count;
            for (i = 0; i < pair_count; i++) {
                //printf("key: <%s>, value: <%s>\n", pairs[i]->key, pairs[i]->value);
                if (0 == thor_config->buffers && 0 == strcmp(BUFFERS, pairs[i]->key)) {
                    thor_config->buffers = atoi(pairs[i]->value);
                    if (thor_config->buffers < 2 || thor_config->buffers > 102399) {
                        //printf("program only supports 2 to 102399 buffers.\n");
                        exit(1);
                    }
                }
                if (NULL == thor_config->output_text_file && 0 == strcmp(OUTPUT_TEXT_FILE, pairs[i]->key)) {
                    thor_config->output_text_file = strdup(pairs[i]->value);
                }
                if (NULL == thor_config->pattern && 0 == strcmp(PATTERN, pairs[i]->key)) {
                    thor_config->pattern  = strdup(pairs[i]->value);
                }
                if (0 == thor_config->do_search && 0 == strcmp(DO_SEARCH, pairs[i]->key)) {
                    thor_config->do_search = atoi(pairs[i]->value);
                }
                if (0 == thor_config->port && 0 == strcmp("port", pairs[i]->key)) {
                    thor_config->port = atoi(pairs[i]->value);
                }
            }
        }
    }
    /* hopefully all sections are filled correctly. Verify that buffers are same for producer and consumer. */
    if (!(0 < jane_config->buffers && jane_config->buffers == thor_config->buffers)) {
        printf("Error: producer and consumer sections dont have same numbers of buffers or there was error while populating it.\n");
        return false;
    }
    if (0 >= jane_config->use_buffers && jane_config->use_buffers > jane_config->buffers) {
        printf("Error: set use buffers value from 1 to max equal to value of \"BUFFERS\".\n");
        return false;
    }
    if ( 0 != thor_config->do_search &&  1 != thor_config->do_search) {
        printf("Error: set DO_SEARCH either value 0 or 1.\n");
        return false;
    }
    if ( 0 == jane_config->port ||  jane_config->port != thor_config->port)  {
        printf("Error: set  same and non zero port..\n");
        return false;
    }
    if ( 1 == thor_config->do_search) {
        assert(NULL != thor_config->pattern);
    }
    return true;
}
/*---------------------------------------------------*/
//#define DEBUG_INI_PARSER
#ifdef DEBUG_INI_PARSER
int main(void)
{
    assert (true == asgard_config_read());
    assert(NULL != jane_config && NULL != thor_config);
    print_jane_config();
    print_thor_config();
}
#endif
/*---------------------------------------------------*/
