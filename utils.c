
#define _POSIX_C_SOURCE 199309L
#include "ft_ping.h"

#include <time.h>


void exit_function(int sig, char *msg)
{
    printf("%s\n", msg);
    exit(sig);
}

char *ft_strdup(char *s1)
{
    size_t len;
    char *dst;

    len = strlen(s1) + 1;
    dst = malloc(len * sizeof(char));
    if (!dst)
        return (NULL);
    memcpy(dst, s1, len);
    return (dst);
}
unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

long get_current_time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.00) + (tv.tv_usec / 1000.00);
}

void handle_sigint(int sig)
{
    double total_time = get_current_time_ms() - g_ping_stats.start_time;
    printf("\n--- %s ping statistics ---\n", g_ping_stats.domain_ip);
    if (g_ping_stats.errors > 0)
    {
        printf("%d packets transmitted, %d received, +%d errors, %.1f%% packet loss, time %.1f ms\n",
               g_ping_stats.packets_sent, g_ping_stats.packets_received, g_ping_stats.errors,
               (g_ping_stats.packets_sent > 0) ? ((float)(g_ping_stats.packets_sent - g_ping_stats.packets_received) / g_ping_stats.packets_sent * 100) : 0.0f,
               total_time);
    }
    else
    {
        printf("%d packets transmitted, %d received, %.1f%% packet loss, time %.1f ms\n",
               g_ping_stats.packets_sent, g_ping_stats.packets_received,
               (g_ping_stats.packets_sent > 0) ? ((float)(g_ping_stats.packets_sent - g_ping_stats.packets_received) / g_ping_stats.packets_sent * 100) : 0.0f,
               total_time);
    }

    exit(0);
}