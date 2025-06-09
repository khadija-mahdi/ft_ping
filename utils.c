
#include "ft_ping.h"

void exit_function(int sig, char *msg)
{
    printf("%s\n", msg);
    exit(sig);
}


char	*ft_strdup(char *s1)
{
	size_t	len;
	char	*dst;

	len = strlen(s1) + 1;
	dst = malloc(len * sizeof(char));
	if (!dst)
		return (NULL);
	memcpy(dst, s1, len);
	return (dst);
}

uint16_t calculate_checksum(void *b, int len) {
    uint16_t *buf = b;
    uint32_t sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len == 1) {
        sum += *(uint8_t *)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}


long get_current_time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}