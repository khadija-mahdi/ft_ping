#ifndef MAIN_H
#define MAIN_H

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>

#define _POSIX_C_SOURCE 200112L

void exitFunction(int sig, char *msg);

#endif