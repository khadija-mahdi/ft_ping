#ifndef PARSING_H
#define PARSING_H


typedef struct
{
    int verbose;
    int show_help;
    char *target;
    char *domain_ip;
} paramters_t;

void print_usage(void);
void ft_ping_help(void);
void parse_args(int argc, char **argv, paramters_t *params);

#endif