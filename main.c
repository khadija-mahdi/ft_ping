#define _POSIX_C_SOURCE 200112L
#include "includes/main.h"
#include "includes/parsing.h"
#include <netdb.h>

void exitFunction(int sig, char *msg)
{
    printf("%s\n", msg);
    exit(sig);
}

void print_usage(void)
{
    printf("Usage: ft_ping [-v] [-?] destination\n");
    exit(0);
}

void ft_ping_help(void)
{
    printf("Usage\n ft_ping [options] destination\n");
    printf("Options:\n");
    printf("  -v        Verbose output\n");
    printf("  -?        Show this help message\n");
    exit(0);
}

void *getDomainIP(char *domain, paramters_t *params)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    int status = getaddrinfo(domain, NULL, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "ft_ping: %s: %s\n", domain, gai_strerror(status));
        exit(1);
    }
    char *ip = inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr);
    if (params->verbose)
    {
        printf("Verbose: Domain %s resolved to IP %s\n", domain, ip);
        printf("Verbose: hints.ai_family: %d\n", hints.ai_family);
        printf("Verbose: ai->ai_family: %d, ai->ai_canonname: '%s'\n", res->ai_family, res->ai_canonname ? res->ai_canonname : "(null)");
    }
    freeaddrinfo(res);
    return ip;
}

void setup_ping(int argc, char **argv, paramters_t *params)
{
    params->verbose = 0;
    params->show_help = 0;
    params->target = NULL;

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        if (arg[0] == '-' && arg[1] != '\0')
        {
            if (strcmp(arg, "-v") == 0)
            {
                params->verbose = 1;
            }
            else if (strcmp(arg, "-?") == 0)
            {
                params->show_help = 1;
                ft_ping_help();
            }
            else
            {
                fprintf(stderr, "ft_ping: invalid option -- '%s'\n", arg);
                ft_ping_help();
            }
        }
        else
        {
            params->target = arg;
            params->domain_ip = getDomainIP(arg, params);
        }
    }

    if (!params->target)
        exitFunction(1, "ft_ping: usage error: Destination address required");
    else
    {
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sockfd < 0)
            exitFunction(1, "Error: Failed to create socket");
    }
}
int main(int argc, char **argv)
{
    paramters_t paramters;
    setup_ping(argc, argv, &paramters);
    printf("IP address for %s: %s\n", paramters.target, paramters.domain_ip);

    return 0;
}