#include "ft_ping.h"

t_ping_stats g_stats = {
    .packets_sent = 0,
    .packets_received = 0,
    .total_rtt = 0,
    .min_rtt = LONG_MAX,
    .max_rtt = 0,
    .start_time = 0,
    .domain_ip = NULL};
void handle_sigint(int sig)
{
    double total_time = get_current_time_ms() - g_stats.start_time;
    printf("\n--- %s ping statistics ---\n", g_stats.domain_ip);
    printf("%d packets transmitted, %d received, %.1f%% packet loss, time %.1f ms\n",
           g_stats.packets_sent, g_stats.packets_received,
           (g_stats.packets_sent > 0) ? ((float)(g_stats.packets_sent - g_stats.packets_received) / g_stats.packets_sent * 100) : 0.0f, total_time);

    if (g_stats.packets_received > 0)
    {
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
               g_stats.min_rtt ,
               (g_stats.total_rtt / g_stats.packets_received),
               g_stats.max_rtt);
    }
    exit(0);
}

void build_icmp_echo_request(struct ft_ping_pkt *pkt, int seq)
{
    memset(pkt, 0, sizeof(*pkt));

    pkt->hdr.type = ICMP_ECHO;
    pkt->hdr.code = 0;
    pkt->hdr.un.echo.id = htons(getpid() & 0xFFFF);
    pkt->hdr.un.echo.sequence = htons(seq);

    for (int i = 0; i < sizeof(pkt->msg); i++)
    {
        pkt->msg[i] = i + 32; // Fill with dummy data
    }
    pkt->hdr.checksum = calculate_checksum(pkt, sizeof(*pkt));
}

void send_ping_request(paramters_t *params, int seq, long *send_time_ms)
{
    struct ft_ping_pkt pkt;
    build_icmp_echo_request(&pkt, seq);

    *send_time_ms = get_current_time_ms();

    ssize_t bytes_sent = sendto(params->sockfd, &pkt, sizeof(pkt), 0,
                                (struct sockaddr *)&params->addr_info,
                                sizeof(params->addr_info));
    if (bytes_sent < 0)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    g_stats.packets_sent++;
}

void receive_ping_response(paramters_t *params, int seq, double send_time_ms)
{
    char buffer[1024];
    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);
    long start_time = get_current_time_ms();
    struct timeval timeout = {3, 0}; // 3 seconds

    struct in_addr target_ip;
    if (inet_aton(params->domain_ip, &target_ip) == 0)
    {
        fprintf(stderr, "Invalid target IP address: %s\n", params->domain_ip);
        exit(EXIT_FAILURE);
    }

    setsockopt(params->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    while (1)
    {
        ssize_t bytes_received = recvfrom(params->sockfd, buffer, sizeof(buffer), 0,
                                          (struct sockaddr *)&reply_addr, &addr_len);

        if (bytes_received < 0)
        {
            if (params->verbose)
                perror("recvfrom failed");
            continue;
        }

        double receive_time_ms = get_current_time_ms();
        struct iphdr *ip = (struct iphdr *)buffer;
        int ip_header_len = ip->ihl * 4;

        if (ip->saddr != target_ip.s_addr)
            continue;

        struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_header_len);
        if (icmp->type == ICMP_ECHOREPLY &&
            ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF) &&
            ntohs(icmp->un.echo.sequence) == seq)
        {
            double rtt = receive_time_ms - send_time_ms;

            g_stats.packets_received++;
            g_stats.total_rtt += rtt;
            if (rtt < g_stats.min_rtt)
                g_stats.min_rtt = rtt;
            if (rtt > g_stats.max_rtt)
                g_stats.max_rtt = rtt;
            printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
                   bytes_received - ip_header_len,
                   inet_ntoa(reply_addr.sin_addr),
                   inet_ntoa(reply_addr.sin_addr),
                   seq,
                   ip->ttl,
                   rtt);
            break;
        }
        else if (icmp->type == ICMP_TIME_EXCEEDED || icmp->type == ICMP_DEST_UNREACH)
        {
            if (params->verbose)
            {
                printf("From %s icmp_seq=%d %s\n",
                       inet_ntoa(reply_addr.sin_addr),
                       seq,
                       icmp->type == ICMP_TIME_EXCEEDED ? "Time to live exceeded" : "Destination Unreachable");
            }
            return;
        }

        if (receive_time_ms - start_time > TIMEOUT_MS)
        {
            printf("Request timed out.\n");
            break;
        }
    }
}

void send_ping(paramters_t *params)
{
    int seq = 1;
    while (1)
    {
        long send_time_ms;
        send_ping_request(params, seq, &send_time_ms);
        receive_ping_response(params, seq, send_time_ms);

        seq++;
        sleep(1);
    }
}

int main(int argc, char **argv)
{
    paramters_t paramters;
    g_stats.start_time = get_current_time_ms();
    setup_ping(argc, argv, &paramters);
    g_stats.domain_ip = paramters.target;
    printf("PING %s (%s) %d(%d) bytes of data.\n",
           paramters.domain_ip,
           paramters.domain_ip,
           ICMP_PAYLOAD_SIZE,
           ICMP_PAYLOAD_SIZE + ICMP_HEADER_SIZE + IP_HEADER_SIZE);
    signal(SIGINT, handle_sigint);
    send_ping(&paramters);
    return 0;
}