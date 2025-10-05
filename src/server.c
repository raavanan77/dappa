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
#include <pthread.h>
#include <sqlite3.h>
#include "vi.h"

#define PORT 7733

struct vi_frame msg;
struct vi_frame recv_data;

static volatile int stop = 1;

char help[512] = "/help - to show this meesage\n/online - to see people online\n/secure {username} - makes encrypted connection with the user\n/reconnect - reconnects users in chat with different encryption key\n/endc - ends connection\n/asecure - enable encryption by default and token refreshed every 24hrs\n";

void killserver();
void error(const char *msg);
static int callback(void *data, int argc, char **argv, char **azColName);
sqlite3 *dbpointer();
int post_clean_up(sqlite3 *db);
int db_manager(sqlite3 *db, void *data, int method);

int main(int argc, char *argv[]){
    /*Declaring variables*/
    int sockfd, portno = PORT;
    struct pollfd fds[10];
    struct connection_list {
        char uuid[16];
        socklen_t clilen;
        struct sockaddr_in cli_addr;
    } con_list[10];
    int user_count = 1;
    pthread_t listen_t, sender_t;
    char listen_buffer[256];
    struct sockaddr_in serv_addr;
    sqlite3 *db = dbpointer();
    struct userlist user;

    if(db == NULL){
        error("DB is null\n");
        return EXIT_FAILURE;
    }

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
    user.leasetime = 0;
    user.dormant = 0;
    // Capturing keyboard signal to stop the server
    signal(SIGINT, killserver);
    while(stop){
        int ready = poll(fds, user_count, -1);

        if(ready > 0){
            for(int i = 0; i < user_count; i++){
                if(fds[i].revents & POLLIN) {
                    if(i == 0){
                        printf("Listening......\n");
                        // Accepts connection on socket and create new socket to read and write
                        int userid;
                        int new_client = accept(sockfd, (struct sockaddr *) &con_list[user_count].cli_addr, &con_list[user_count].clilen);
                        fds[user_count].fd = new_client;
                        fds[user_count].events = POLLIN;
                        int listen = read(fds[user_count].fd, user.username, 16);
                        int dbreturn_code = db_manager(db, &user, 9);
                        if(dbreturn_code == 1){
                            dbreturn_code = db_manager(db, &user, 11);
                            if(dbreturn_code == 0){
                                dbreturn_code = db_manager(db, &user, 9);
                                if(dbreturn_code==0){
                                    int usersize = 0;
                                    printf("\n == new login UUID: %d == \n", usersize);
                                    listen = write(fds[user_count].fd, &usersize, sizeof(int));
                                    printf("Write :%d\n", listen);
                                }
                            }
                        }
                        else{
                            int usersize = 1;
                            printf("\n == old login UUID: %d == \n", usersize);
                            listen = write(fds[user_count].fd, &usersize, sizeof(int));
                            printf("old Write :%d\n", listen);
                        }
                        strcpy(con_list[user_count].uuid, user.username);
                        
                        user_count++;
                        printf("New Client connected at socket : %d And the UUID : %d\n", new_client, con_list[user_count].uuid);
                        fflush(stdout);
                    }
                    else{
                        unsigned char buffered[sizeof(recv_data)];
                        int read_byte = read(fds[i].fd, recv_data.suuid, 16);
                        if(read_byte > 0){
                            read_byte = read(fds[i].fd, recv_data.duuid, 16);
                            read_byte = read(fds[i].fd, &recv_data.payload_size, sizeof(int));
                            printf("SUUID : %s DUUID : %s\n", recv_data.suuid, recv_data.duuid);
                            for(int z = 1; z < 3; z++){
                                printf("CON_LIST of %d is %d\n", z, con_list[z].uuid);
                                if(strcmp(con_list[z].uuid, recv_data.duuid) == 0){
                                    recv_data.payload = malloc(recv_data.payload_size+1);
                                    read_byte = read(fds[i].fd, &recv_data.timestamp, sizeof(time_t));
                                    read_byte = read(fds[i].fd, recv_data.payload, recv_data.payload_size);
                                    recv_data.payload[recv_data.payload_size] = '\0';
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

void killserver(){
    stop = 0;
    printf("Server Terminated Exit code : 77\n");
}

void error(const char *msg){
    perror(msg);
    exit(1);
}

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    struct userlist* user = (struct userlist*)data;
    strcpy(user->username, argv[1]);
    user->uuid = atoi(argv[0]);
    user->leasetime = atoi(argv[2]);
    user->dormant = atoi(argv[3]);
    
    return 0;
}

sqlite3 *dbpointer(){
    // SQLite vars
    sqlite3 *db;
    char db_path[256];
    snprintf(db_path, sizeof(db_path), "%s/.config/dappa/server.db", getenv("HOME"));

    int return_code = sqlite3_open(db_path, &db);
    if(return_code != 0){
        error("SQLite died\n");
        return NULL;
    }
    return db;
}

int post_clean_up(sqlite3 *db){
    int dbreturn_code = sqlite3_close(db);
    if(dbreturn_code == SQLITE_OK){
        return 0;
    }
    return -1;
}

int db_manager(sqlite3 *db, void *data, int method){
    /*
    method code
    9 - Find user
    11 - INSERT
    12 - SELECT
    13 - DELETE
    14 - UPDATE
    */

    // SQLite vars
    int return_code;
    string errmsg = 0;
    sqlite3_stmt *stmt;
    struct userlist* user = (struct userlist*)data;
    string username = malloc(strlen(user->username)+1);
    strcpy(username, user->username);
    //user = NULL;
    
    switch (method){
        case 11:
            sqlite3_prepare_v2(db, INSERT_USER, -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, user->username, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, user->leasetime);
            sqlite3_bind_int(stmt, 3, user->dormant);
            return_code = sqlite3_step(stmt);
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                return SQLITE_ERROR;
            }
            sqlite3_finalize(stmt);
            return 0;
            break;
        case 9:
            string indbuser = malloc(sizeof(username));
            sqlite3_prepare_v2(db, "SELECT * FROM userlist WHERE TRIM(username) = ?", -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
            while((return_code = sqlite3_step(stmt)) == SQLITE_ROW){
                user->uuid = sqlite3_column_int(stmt, 0);
                const char* db_username = (const char*)sqlite3_column_text(stmt, 1);
                strncpy(user->username, db_username, 16);
                strcpy(indbuser, db_username);
                user->leasetime = sqlite3_column_int(stmt, 2);
                user->dormant = sqlite3_column_int(stmt, 3);
            }
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                //sqlite3_finalize(stmt);
                return SQLITE_ERROR;
            }
            //sqlite3_finalize(stmt);
            printf("User :%s\n", indbuser);
            if(strcmp(username, indbuser) == 0){
                return 0;
            }
            else{
                return 1;
            }
            free(indbuser);
            break;
        default:
            break;
    }
    free(username);
}
