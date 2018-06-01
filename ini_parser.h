#pragma once
typedef struct producer_config {
    char *input_text_file;
    int buffers;
    int use_buffers;
    int port;
} Jane_config;

typedef struct consumer_config {
    char *output_text_file;
    char *pattern;
    int buffers;
    int do_search;
    int port;
} Thor_config;

extern Jane_config *jane_config;
extern Thor_config *thor_config;

Jane_config *get_jane_config(void);
Thor_config *get_thor_config(void);
bool asgard_config_read(void);
