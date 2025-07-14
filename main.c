#include "ft_ping.h"

int main(int argc, char **argv)
{
    paramters_t paramters;
    g_ping_stats.start_time = get_current_time_ms();
    setup_ping(argc, argv, &paramters);
    signal(SIGINT, handle_sigint);
    send_ping(&paramters);
    return 0;
}