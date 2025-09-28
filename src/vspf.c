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
#include <sys/poll.h>
#include <arpa/inet.h>
#include <linux/ip.h>

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
    struct pollfd fds[1];
    socklen_t nodelen = sizeof(root_addr);
    struct vi_proto header;                 // Packet header
    unsigned char buffer[sizeof(header)+20];   // Buffer stream
    int node_count = 1;

    if(sockfd == -1){
        error("well try again \n");
    }
    root_addr.sin_family = AF_INET;
    root_addr.sin_addr.s_addr = INADDR_ANY;
    root_addr.sin_port = htons(PORT);
    int binfd = bind(sockfd, (struct sockaddr *) &root_addr, sizeof(root_addr));
    if( binfd < 0){
        error("Well bind not binding\n");
    }
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    while(stop){
        int onpoll = poll(fds, 1, 1000);
        if(onpoll > 0){
            ssize_t bytes = recvfrom(sockfd, buffer, sizeof(buffer) , 0, (struct sockaddr*) &root_addr, &nodelen);
            struct iphdr *ip = (struct iphdr *)buffer;
            printf("Dest: %X\n", ip->daddr);
            int ip_len = ip->ihl * 4;
            printf("Got packet from %s\n", inet_ntoa(root_addr.sin_addr));
            proto_stream_buffer(buffer, &header, ip_len);
            //memcpy(&header, (struct vi_proto *)&buffer + ip_len, sizeof(header));
            printf("SRC : %u\n", header.src);
            if(header.isdest == 1 && (header.nhop, inet_ntoa(root_addr.sin_addr)) == 0){
                printf("Now\n");
            }
            fflush(stdout);
        }   
    }
    close(fds[0].fd);
}

void *clientnode(){
    int nodefd, portno;                     // Socket and port
    struct sockaddr_in node_addr;           // IP struct
    socklen_t rootlen = sizeof(node_addr);
    struct vi_proto header;                 // Packet header
    unsigned char buffer[53];   // Buffer stream
    int node_count = 1;
    string hostname = "192.168.204.195";
    string dest= "192.168.204.1";
    header.src = inet_addr(hostname);
    header.dest = inet_addr(dest);
    header.nhop = inet_addr(dest);
    header.isdest = 1;
    proto_stream_buffer(buffer, &header, 0);
    printf("SRC : %u\n", header.src);
    nodefd = socket(AF_INET, SOCK_RAW, VIPROTO_ROUTING);
    if(nodefd < 0){
        error("well try again \n");
    }
    node_addr.sin_family = AF_INET;
    node_addr.sin_addr.s_addr = inet_addr(hostname);
    node_addr.sin_port = htons(PORT);
    //memcpy(&buffer, hostname, sizeof(hostname));
    int con = sendto(nodefd, buffer, sizeof(buffer), 0, (struct sockaddr*) &node_addr, rootlen);
    if(con < 0){
        error("Well connect failed\n");
    }
    else{
        printf("Connected : %d\n", con);
    }
    
    close(nodefd);
}

int main(int argc, string argv[]){
    pthread_t server_t, node_t;
    
    signal(SIGINT, shutoff);
    if (argc > 0 ) {
        if(atoi(argv[1]) == 0){
            pthread_create(&server_t, NULL, server, NULL);
            pthread_join(server_t, NULL);
        }
        else{
            pthread_create(&node_t, NULL, clientnode, NULL);
            pthread_join(node_t, NULL);
        }
    }
    
    return 0;
}
