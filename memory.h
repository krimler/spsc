#pragma once

#include <stdint.h>
#include <stdlib.h>
typedef enum MRole {
    M_JANE,
    M_THOR
} MRole;

typedef enum MStatus {
    M_INIT,
    M_STARTED
} MStatus;
typedef struct Ms {
    uint64_t sz ;
    uint32_t pattern;
    key_t key;
    MRole role;
    /*internal*/
    char *shm;
    int shmid;
} Ms;

int minit(Ms  *ms);
