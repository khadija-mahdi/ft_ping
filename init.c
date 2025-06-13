
#include "ft_ping.h"

void ft_ping_help(void)
{
    printf("Usage\n ft_ping [options] destination\n");
    printf("Options:\n");
    printf("  -v        Verbose output\n");
    printf("  -? and -h       Show this help message\n");
    exit(0);
}

void parse_option(char *arg, paramters_t *params)
{
    if (strcmp(arg, "-v") == 0)
        params->verbose = 1;
    else if (strcmp(arg, "-?") == 0 || strcmp(arg, "-h") == 0)
    {
        params->show_help = 1;
        ft_ping_help();
        exit(0);
    }
    // Add bonus options here
    else
    {
        fprintf(stderr, "ft_ping: invalid option -- '%s'\n", arg);
        ft_ping_help();
        exit(1);
    }
}

void getDomainIP(char *domain, paramters_t *params)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    int status = getaddrinfo(domain, NULL, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "ft_ping: %s: %s\n", domain, gai_strerror(status));
        exit(1);
    }
    char *ip = inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr);
    free(params->domain_ip);
    params->domain_ip = ft_strdup(ip);

    // Set addr_info.sin_addr to the resolved IP
    params->addr_info.sin_family = AF_INET;
    params->addr_info.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;

    if (res->ai_canonname)
        params->canonname = ft_strdup(res->ai_canonname);
    else
        params->canonname = ft_strdup(domain);
    freeaddrinfo(res);
}

void parseArgsGetDomainIP(int argc, char **argv, paramters_t *params)
{
    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (arg[0] == '-' && arg[1] != '\0')
            parse_option(arg, params);
    }

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (!(arg[0] == '-' && arg[1] != '\0'))
        {
            params->target = arg;
            getDomainIP(arg, params);
        }
    }

    if (!params->target)
        exit_function(1, "ft_ping: usage error: Destination address required");
}

void setup_ping(int argc, char **argv, paramters_t *params)
{
    params->verbose = 0;
    params->show_help = 0;
    params->target = NULL;
    params->domain_ip = NULL;
    parseArgsGetDomainIP(argc, argv, params);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    params->sockfd = sockfd;
    if (params->verbose)
    {
        printf("ft_ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", params->sockfd);
        printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", params->canonname);
    }
}