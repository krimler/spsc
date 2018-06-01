#pragma once
#include <arpa/inet.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <openssl/crypto.h>
#include <openssl/err.h>

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#include <assert.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include<string.h>
#include <sys/ipc.h>
#include <sys/time.h>

#include "memory.h"
#include "log.h"
#include "asgard.h"
#include "ini_parser.h"
#include "substring.h"
#include "q.h"

#define ERR_OK 0
#define ERR_OCCURED 1

