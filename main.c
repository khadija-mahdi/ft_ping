#include "ft_ping.h"
#include <netinet/ip.h> // For struct iphdr

void build_icmp_echo_request(struct ft_ping_pkt *pkt, int seq)
{
    memset(pkt, 0, sizeof(*pkt));

    pkt->hdr.type = ICMP_ECHO;
    pkt->hdr.code = 0;
    pkt->hdr.un.echo.id = htons(getpid() & 0xFFFF);
    pkt->hdr.un.echo.sequence = htons(seq);

    for (int i = 0; i < sizeof(pkt->msg); i++)
    {
        pkt->msg[i] = i + 32;
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
}

void receive_ping_response(paramters_t *params, int seq, long send_time_ms)
{
    char buffer[1024];
    struct iphdr *ip = (struct iphdr *)buffer;
    struct in_addr src_addr;
    src_addr.s_addr = ip->saddr;
    printf("Packet source IP (from IP header): %s\n", inet_ntoa(src_addr));

    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);
    long start_time = get_current_time_ms();

    while (1)
    {
        ssize_t bytes_received = recvfrom(params->sockfd, buffer, sizeof(buffer), 0,
                                          (struct sockaddr *)&reply_addr, &addr_len);
        if (bytes_received < 0)
        {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        long receive_time_ms = get_current_time_ms();

        struct iphdr *ip = (struct iphdr *)buffer;
        int ip_header_len = ip->ihl * 4;

        if (bytes_received < ip_header_len + sizeof(struct icmphdr))
        {
            // Packet too small to contain ICMP header; ignore
            continue;
        }

        struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_header_len);
        printf("Received ICMP packet: type=%d, id=%u, seq=%u (host order: id=%u, seq=%u)\n",
               icmp->type,
               ntohs(icmp->un.echo.id),
               ntohs(icmp->un.echo.sequence),
               ntohs(icmp->un.echo.id),
               ntohs(icmp->un.echo.sequence));

        if (icmp->type == ICMP_ECHOREPLY &&
            ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF) &&
            ntohs(icmp->un.echo.sequence) == seq)
        {
            long rtt = receive_time_ms - send_time_ms;
            printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%ld ms\n",
                   inet_ntoa(reply_addr.sin_addr),
                   seq,
                   ip->ttl,
                   rtt);
            break;
        }
        else
        {
            printf("Received non-echo-reply or mismatched ID/seq, ignoring...\n");
        }

        // Optional: timeout to avoid infinite blocking
        if (receive_time_ms - start_time > 3000) // 3 seconds timeout
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
    setup_ping(argc, argv, &paramters);

    printf("IP address for %s: %s\n", paramters.target, paramters.domain_ip);
    send_ping(&paramters);
    return 0;
}