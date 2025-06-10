#ifndef FT_PING_H
#define FT_PING_H

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
#include <sys/time.h>
#include <netinet/ip.h> 
#include <limits.h> 

#define ICMP_PAYLOAD_SIZE 56
#define ICMP_HEADER_SIZE 8
#define IP_HEADER_SIZE 20
#define TIMEOUT_MS 3000

typedef struct
{
    int sockfd;
    int verbose;
    int show_help;
    char *target;
    char *domain_ip;
    char *canonname;
    struct sockaddr_in addr_info;
} paramters_t;

struct __attribute__((packed)) ft_ping_pkt
{
    struct icmphdr hdr;
    char msg[ICMP_PAYLOAD_SIZE];
};

void setup_ping(int argc, char **argv, paramters_t *params);
uint16_t calculate_checksum(void *b, int len);
void exit_function(int sig, char *msg);
char *ft_strdup(char *s1);
long get_current_time_ms();

#endif