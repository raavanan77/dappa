#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include "vi.h"

// typedefs
typedef char* string;

// Constants
#define PORT 3003
#define VIPROTO_ROUTING 253

static volatile int stop = 1;

void shutoff(){
    stop = 0;
    printf("Node is about to shutdown\n");
}

void error(const string msg){
    perror(msg);
    exit(1);
}

void *server(){
    int sockfd, portno;                     // Socket and port
    struct sockaddr_in root_addr;           // IP struct
    struct neigh_node
    {
        socklen_t nodelen;
        struct sockaddr_in node_addr;
    } nodes[7];
    struct vi_proto header;                 // Packet header
    unsigned char buffer[sizeof(header)];   // Buffer stream

    sockfd = socket(AF_INET, SOCK_RAW, VIPROTO_ROUTING);
    if(sockfd < 0){
        error("well try again \n");
    }
    root_addr.sin_family = AF_INET;
    root_addr.sin_addr.s_addr = INADDR_ANY;
    root_addr.sin_port = htons(PORT);

    if(bind(sockfd, (struct sockaddr *) &root_addr, sizeof(root_addr)) < 0){
        error("Well bind not binding\n");
    }
    listen(sockfd, 10);
    
    while(stop){
        
    }
    close(sockfd);
}

int main(){
    pthread_t server_t, node_t;
    
    signal(SIGINT, shutoff);
    pthread_create(&server_t, NULL, server, NULL);
    pthread_join(server_t, NULL);
    
    return 0;
}