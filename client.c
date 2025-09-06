#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "caesar.h"
#include <pthread.h>
#include <signal.h>


static volatile int stop = 1;

void killserver(){
    stop = 0;
    printf("Server Terminated Exit code : 77\n");
}

void error(const char *msg){
    perror(msg);
    exit(0);
}

void *listener(void *nsock){
    int listen;
    int sock = *(int *) nsock;
    char listen_buffer[256];
    while(stop) {
        if (stop == 0) break;
        bzero(listen_buffer, 256);
        listen = read(sock, listen_buffer, 255);
        if(listen <= 0) break;
        
        listen_buffer[listen] = '\0';
        caesar_decrypt(-13, listen_buffer);
        printf("Client says: %s", listen_buffer);
    }
}

void *sender(void *nsock){
    int send;
    int sock = *(int *) nsock;
    char sender_buffer[256];
    while(stop) {
        if (stop == 0) break;
        printf("Enter your message: ");
        bzero(sender_buffer, 256);
        fgets(sender_buffer, 255, stdin);
        if(sender_buffer > 0){
            printf("is close : %s\n", sender_buffer);
            if(strcmp(sender_buffer, "/close\n") == 0){
                //break;
                exit(1);
                //stop = 0;
            }
        }
        caesar_encrypt(13, sender_buffer);
        send = write(sock, sender_buffer, strlen(sender_buffer));
        if(send < 0) error("ERROR : Writing to socket\n");
    }
}

int main(int argc, char *argv[]){
    int sockfd, portno, n;
    char* hostname;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t listen_t, sender_t;
    char buffer[256];
    if(argc < 3){
        hostname = "127.0.0.1";
        portno = 7733;
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        //exit(0);
    }
    else{
        hostname = argv[1];
        portno = atoi(argv[2]);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR: Opening socket\n");
    server = gethostbyname(hostname);
    if(server == NULL){
        fprintf(stderr, "ERROR: No such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR: Connecting...");
    signal(SIGINT, killserver);
    pthread_create(&listen_t, NULL, listener, &sockfd);
    pthread_create(&sender_t, NULL, sender, &sockfd);
    pthread_join(listen_t, NULL);
    pthread_join(sender_t, NULL);
    //while(1){
    //    printf("Enter message : ");
    //    bzero(buffer, 256);
    //    fgets(buffer, 255, stdin);
    //    caesar_encrypt(13, buffer);
    //    n = write(sockfd, buffer, strlen(buffer));
    //    if(n<0) error("ERROR: Write to socket\n");
    //    bzero(buffer, 256);
    //    n = read(sockfd, buffer, 255);
    //    if(n < 0){
    //        error("ERROR reading from socket");
    //    }
    //    caesar_decrypt(-13, buffer);
    //    printf("Server says: %s\n", buffer);
    //}
    close(sockfd);
    return 0;
}