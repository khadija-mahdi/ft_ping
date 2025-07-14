#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include "ft_ping.h"

void build_icmp_echo_request(struct ft_ping_pkt *pkt, int seq, paramters_t *params)
{
    memset(pkt, 0, sizeof(*pkt));
    pkt->hdr.type = ICMP_ECHO;
    pkt->hdr.code = 0;
    pkt->hdr.un.echo.id = htons(getpid() & 0xFFFF);
    pkt->hdr.un.echo.sequence = htons(seq);

    struct timeval temp_timestamp;
    gettimeofday(&temp_timestamp, NULL);
    params->timestamp = temp_timestamp;

    for (int i = 0; i < sizeof(pkt->msg); i++)
        pkt->msg[i] = i + 32;

    pkt->hdr.checksum = calculate_checksum(pkt, sizeof(*pkt));
}

void send_ping_request(paramters_t *params, int seq)
{
    struct ft_ping_pkt pkt;
    build_icmp_echo_request(&pkt, seq, params);
    if (params->ttl_set)
    {
        if (setsockopt(params->sockfd, IPPROTO_IP, IP_TTL, &params->ttl, sizeof(params->ttl)) < 0)
        {
            perror("ping: cannot set unicast time-to-live:");
            exit(EXIT_FAILURE);
        }
    }
    ssize_t bytes_sent = sendto(params->sockfd, &pkt, sizeof(pkt), 0,
                                (struct sockaddr *)&params->addr_info,
                                sizeof(params->addr_info));
    if (bytes_sent < 0)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    g_ping_stats.packets_sent++;
}

void receive_ping_response(paramters_t *params, int seq)
{
    char buffer[1024];
    char control[1024];
    struct sockaddr_in reply_addr;
    struct msghdr msg;
    struct iovec iov;
    socklen_t addr_len = sizeof(reply_addr);

    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &reply_addr;
    msg.msg_namelen = addr_len;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    struct timeval start, now;
    gettimeofday(&start, NULL);

    while (1)
    {
        gettimeofday(&now, NULL);
        long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
        if (elapsed_ms >= 1000)
            break;

        ssize_t bytes_received = recvmsg(params->sockfd, &msg, MSG_DONTWAIT);
        if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            break;
        }

        struct iphdr *ip = (struct iphdr *)buffer;
        int ip_header_len = ip->ihl * 4;
        struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_header_len);

        if (icmp->type == ICMP_ECHOREPLY &&
            ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF) &&
            ntohs(icmp->un.echo.sequence) == seq)
        {
            struct timeval recv_time;
            gettimeofday(&recv_time, NULL);
            struct ft_ping_pkt *pkt = (struct ft_ping_pkt *)(buffer + ip_header_len);
            double rtt = (recv_time.tv_sec - params->timestamp.tv_sec) * 1000.0 +
                         (recv_time.tv_usec - params->timestamp.tv_usec) / 1000.0;
            int Packet_size = bytes_received - ip_header_len;
            char *ip_str = inet_ntoa(reply_addr.sin_addr);

            if (params->verbose)
            {
                printf("%d bytes from %s (%s): icmp_seq=%d ident=%d ttl=%d time=%.3f ms\n",
                       Packet_size,
                       ip_str,
                       ip_str,
                       seq,
                       ntohs(icmp->un.echo.id),
                       ip->ttl,
                       rtt);
            }
            else
            {
                printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3f ms\n",
                       Packet_size,
                       ip_str,
                       ip_str,
                       seq,
                       ip->ttl,
                       rtt);
            }

            g_ping_stats.packets_received++;
            g_ping_stats.total_rtt += rtt;
            if (rtt < g_ping_stats.min_rtt)
                g_ping_stats.min_rtt = rtt;
            if (rtt > g_ping_stats.max_rtt)
                g_ping_stats.max_rtt = rtt;
            break;
        }
        else if (icmp->type == ICMP_TIME_EXCEEDED || icmp->type == ICMP_DEST_UNREACH)
        {
            g_ping_stats.errors++;
            if (params->verbose)
            {
                printf("From %s (%s) icmp_seq=%d %s\n",
                       inet_ntoa(reply_addr.sin_addr),
                       inet_ntoa(reply_addr.sin_addr),
                       seq,
                       icmp->type == ICMP_TIME_EXCEEDED ? "Time to live exceeded" : "Destination Unreachable");
            }
            return;
        }
    }
}

void send_ping(paramters_t *params)
{
    int seq = 1;
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (1)
    {

        send_ping_request(params, seq);
        receive_ping_response(params, seq);
        seq++;

        next.tv_sec += 1;

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
}
