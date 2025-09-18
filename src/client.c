#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "vi.h"

static volatile int stop = 1;

struct vi_frame msg;
struct vi_frame recv_data;
struct tm *ltime;

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
    int header[3];
    int sock = *(int *) nsock;
    char listen_buffer[256];
    while(stop) {
        if (stop == 0) break;
        time_t timestamp;
        unsigned char buffered[sizeof(recv_data)];
        listen = read(sock, header, sizeof(header));
        if(listen > 0){
            recv_data.suuid = header[0];
            recv_data.duuid = header[1];
            recv_data.payload_size = header[2];
            if(recv_data.payload_size > 0){
                listen = read(sock, &timestamp, sizeof(time_t));
                recv_data.timestamp = timestamp;
                ltime = localtime(&timestamp);
                printf("Time : %s => ", asctime(ltime));
                recv_data.payload = malloc(recv_data.payload_size+1);
                listen = read(sock, recv_data.payload, recv_data.payload_size);
                recv_data.payload[recv_data.payload_size] = '\0';
                caesar_decrypt(-13, recv_data.payload);
                printf("Client says: %s\n", recv_data.payload);
                free(recv_data.payload);
            }
        }
        if(listen <= 0) break;
    }
}

void *sender(void *nsock){
    int send;
    int sock = *(int *) nsock;
    char buffer[256];
    unsigned char bystream[sizeof(msg)];
    while(stop) {
        if (stop == 0) break;
        printf("Enter your message: ");
        memset(buffer, 0, 256);
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if(strlen(buffer) > 0 && strcmp(buffer,"/close") == 0){
            close(sock);
            stop = 0;
            exit(1);
        }
        msg.payload_size = strlen(buffer) + 1;
        msg.payload = malloc(msg.payload_size);  // +1 for null terminator
        if (msg.payload == NULL) {
            // Handle allocation failure
            fprintf(stderr, "Memory allocation failed\n");
            //return -1;
        }
        strcpy(msg.payload, buffer);
        caesar_encrypt(13, msg.payload);
        msg.timestamp = time(NULL);
        int byte_len = stream_buffer(bystream, &msg);
        send = write(sock, bystream, byte_len);
        free(msg.payload);
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
        msg.suuid = 3003;
        msg.duuid = 3006;
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        //exit(0);
    }
    else{
        hostname = argv[1];
        portno = atoi(argv[2]);
        msg.suuid = 3006;
        msg.duuid = 3003;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR: Opening socket\n");
    server = gethostbyname(hostname);
    if(server == NULL){
        fprintf(stderr, "ERROR: No such host\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr_list[0],(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR: Connecting...");
    //int send = write(sockfd, &msg.suuid, 4);
    signal(SIGINT, killserver);
    printf("DUUID set to %d\n", msg.duuid);
    pthread_create(&listen_t, NULL, listener, &sockfd);
    pthread_create(&sender_t, NULL, sender, &sockfd);
    pthread_join(listen_t, NULL);
    pthread_join(sender_t, NULL);
    close(sockfd);
    return 0;
}
