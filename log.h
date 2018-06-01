#pragma once
#include "util.h"
#define INFO   0x1
#define DEBUG  0x2
extern volatile int ts_lock;
extern uint32_t LOG_PRIORITY;
#define INDEX_LOG_STRING "CROWDSTRIKE"

#define LEVEL(prio) \
        (prio == INFO ?   "[INFO]" : "[DEBUG]")

/*
 * The LOG facility requires data in below format:
 *
 * LOG(<LOG_LEVEL>, <LOG_ID> ,"FORMAT STRING",<arguments seperated by commas>)
 * LOG_LEVEL can be in INFO or DEBUG
 *    But as such there are no checks to verify.
 * e.g. LOG(INFO, "%s", foo);
 */
#define LOG(prio, args...)                              \
    do {                                                \
        lock(&ts_lock);                                 \
        if (prio <= LOG_PRIORITY) {                     \
            printf("%s:%d - ", __FUNCTION__, __LINE__)  \
            printf(args);                               \
            printf("\r\n");                             \
        }                                               \
        unlock(&ts_lock);                               \
    } while(0);

#define LOGI(args...)                                                   \
    do {                                                                \
        lock(&ts_lock);                                                 \
        if (INFO <= LOG_PRIORITY) {                                     \
            printf("%s %s:%d - ", TimeStamp(), __FUNCTION__, __LINE__); \
            printf(args);                                               \
            printf("\r\n");                                             \
        }                                                               \
        unlock(&ts_lock);                                               \
    } while(0);

#define LOGD(args...)                                                   \
    do {                                                                \
        lock(&ts_lock);                                                 \
        if (DEBUG <= LOG_PRIORITY) {                                    \
            printf("%s %s:%d - ", TimeStamp(), __FUNCTION__, __LINE__); \
            printf(args);                                               \
            printf("\r\n");                                             \
        }                                                               \
        unlock(&ts_lock);                                               \
    } while(0);
