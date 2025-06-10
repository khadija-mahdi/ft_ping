
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
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}