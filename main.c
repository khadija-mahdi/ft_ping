#include "includes/main.h"

// AF_INET for IPv4

// SOCK_RAW for raw sockets

// IPPROTO_ICMP to specify you're sending ICMP packets

void parse_args(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
}

void sendCtmRequest() {

}

int main(int argc, char **argv){
    parse_args(argc, argv);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }
    return 0;
}