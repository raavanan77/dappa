#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "caesar.h"

typedef char* string;

static volatile int stop = 1;

char help[512] = "/help - to show this meesage\n/online - to see people online\n/secure {username} - makes encrypted connection with the user\n/reconnect - reconnects users in chat with different encryption key\n/endc - ends connection\n/asecure - enable encryption by default and token refreshed every 24hrs\n";

void whosup(); // Returns people who is online

void command_resolver(); // Resolve and calls the user command

void killserver(){
    stop = 0;
    printf("Server Terminated Exit code : 77\n");
}

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int sockfd, nsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if(argc < 2){
        portno = 7733; // default port
        fprintf(stderr,"ERROR, No port given. Using default port: 7733\n");
        //exit(1);
    }
    else{
        portno = atoi(argv[1]);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR : Bind failed\n");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    nsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(nsockfd < 0){
        error("ERROR : Accept failed\n");
    }
    signal(SIGINT, killserver);
    while(stop) {
        if (stop == 0) break;
        bzero(buffer, 256);
        n = read(nsockfd, buffer, 255);
        if(n <= 0) break;
        
        buffer[n] = '\0';
        caesar_decrypt(-13, buffer);
        printf("Client says: %s", buffer);
        if(strcmp(buffer,"/help") == 1){
            caesar_encrypt(13, help);
            n = write(nsockfd, help, strlen(help));
            continue;
        }
        printf("Enter your message: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        caesar_encrypt(13, buffer);
        n = write(nsockfd, buffer, strlen(buffer));
        if(n < 0) error("ERROR : Writing to socket\n");
    }
    
    close(nsockfd);
    close(sockfd);
    return 0;
}