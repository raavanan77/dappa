#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "caesar.h"
#include <pthread.h>
#include "vi.h"

#define PORT 7733

typedef char* string;

struct vi_frame msg;
struct vi_frame recv_data;

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
    /*Declaring variables*/
    int sockfd, portno = PORT;
    struct pollfd fds[10];
    struct connection_list {
        int uuid;
        socklen_t clilen;
        struct sockaddr_in cli_addr;
    } con_list[10];
    int user_count = 1;
    pthread_t listen_t, sender_t;
    char listen_buffer[256];
    struct sockaddr_in serv_addr;
    int n;

    if(argc < 2){
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

    memset(&con_list, 0, sizeof(con_list));
    
    // Assigning address and port for socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    // Binds socket
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR : Bind failed\n");
    }

    // Listening on newly created socket
    listen(sockfd, 5);
    con_list[user_count].clilen = sizeof(con_list[user_count].cli_addr);

    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    // Capturing keyboard signal to stop the server
    signal(SIGINT, killserver);
    int uuid = 3003;
    while(stop){
        int ready = poll(fds, user_count, -1);

        if(ready > 0){
            for(int i = 0; i < user_count; i++){
                if(fds[i].revents & POLLIN) {
                    if(i == 0){
                        // Accepts connection on socket and create new socket to read and write
                        int userid;
                        int new_client = accept(sockfd, (struct sockaddr *) &con_list[user_count].cli_addr, &con_list[user_count].clilen);
                        fds[user_count].fd = new_client;
                        fds[user_count].events = POLLIN;
                        con_list[user_count].uuid = uuid;
                        uuid+=3;
                        user_count++;
                        //int listen = read(fds[user_count].fd, &con_list[user_count].uuid, 4);
                        printf("New Client connected at socket : %d And the UUID : %d\n", new_client, con_list[user_count].uuid);
                        fflush(stdout);
                    }
                    else{
                        int header[3];
                        unsigned char buffered[sizeof(recv_data)];
                        int read_byte = read(fds[i].fd, header, sizeof(header));
                        if(read_byte > 0){
                            recv_data.suuid = header[0];
                            recv_data.duuid = header[1];
                            recv_data.payload_size = header[2];
                            printf("SUUID : %d DUUID : %d\n", recv_data.suuid, recv_data.duuid);
                            for(int z = 1; z < 3; z++){
                                printf("CON_LIST of %d is %d\n", z, con_list[z].uuid);
                                if(con_list[z].uuid == recv_data.duuid){
                                    recv_data.payload = malloc(recv_data.payload_size+1);
                                    read_byte = read(fds[i].fd, recv_data.payload, recv_data.payload_size);
                                    recv_data.payload[recv_data.payload_size] = '\0';
                                    int offset = 0;
                                    int byte_len = stream_buffer(buffered, &recv_data);
                                    int send = write(fds[z].fd, buffered, byte_len);
                                    send < 0 ? printf("ERROR routing msg\n") : printf("Routed msg to %d\n", recv_data.duuid) ;
                                    free(recv_data.payload);
                                }
                            }
                            fflush(stdout);
                        }
                        else{
                            close(fds[i].fd);
                            for(int j = i; j < user_count - 1; j++){
                                fds[j] = fds[j+1];
                            }
                            user_count--;
                            i--;
                        }
                    }
                }
            }
        }
    }
    close(sockfd);
    return 0;
}
