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
#include <sqlite3.h>
#include "vi.h"

typedef char* string;

static volatile int stop = 1;
struct vi_frame msg;
struct vi_frame recv_data;
struct tm *ltime;

struct pthread_args{
    int socket;
    sqlite3 *db;
};

void killserver();
void error(const char *msg);
int signin();
void *listener(void *nsock);
void *sender(void *nsock);
static int callback(void *NU, int argc, char **argv, char **azColName);
sqlite3 *dbpointer();
int post_clean_up(sqlite3 *db);
int db_manager(sqlite3 *db, struct vi_frame* message, int method);

int main(int argc, char *argv[]){
    //socket vars
    int sockfd, portno, n;
    string hostname;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t listen_t, sender_t;
    char buffer[256];
    sqlite3 *db = dbpointer();
    struct pthread_args args;
    
    if(db == NULL){
        error("DB is null\n");
        return EXIT_FAILURE;
    }

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
    bcopy((string) server->h_addr_list[0],(string)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR: Connecting...");
    //int send = write(sockfd, &msg.suuid, 4);
    signal(SIGINT, killserver);
    printf("DUUID set to %d\n", msg.duuid);
    args.socket = sockfd;
    args.db = db;
    pthread_create(&listen_t, NULL, listener, &args);
    pthread_create(&sender_t, NULL, sender, &args);
    pthread_join(listen_t, NULL);
    pthread_join(sender_t, NULL);
    post_clean_up(db);
    close(sockfd);
    return 0;
}

void killserver(){
    stop = 0;
    printf("Server Terminated Exit code : 77\n");
}

void error(const char *msg){
    perror(msg);
    exit(0);
}

static int callback(void *NU, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s == %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

sqlite3 *dbpointer(){
    // SQLite vars
    sqlite3 *db;
    char db_path[256];
    snprintf(db_path, sizeof(db_path), "%s/.config/dappa/storage.db", getenv("HOME"));

    int return_code = sqlite3_open(db_path, &db);
    if(return_code != 0){
        error("SQLite died\n");
        return NULL;
    }
    return db;
}

int post_clean_up(sqlite3 *db){
    int rc = sqlite3_close(db);
    if(rc == SQLITE_OK){
        return 0;
    }
    return -1;
}

int db_manager(sqlite3 *db, struct vi_frame* message, int method){
    /*
    method code
    11 - INSERT
    12 - SELECT
    13 - DELETE
    14 - UPDATE
    */

    // SQLite vars
    int return_code;
    string errmsg = 0;
    sqlite3_stmt *stmt;

    switch (method){
        case 11:
            sqlite3_prepare_v2(db, INSERT_MSG, -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, message->suuid);
            sqlite3_bind_int(stmt, 2, message->duuid);
            sqlite3_bind_text(stmt, 3, message->payload, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 4, message->timestamp);
            return_code = sqlite3_step(stmt);   
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                return SQLITE_ERROR;
            }
            sqlite3_finalize(stmt);
            return 0;
            break;
        case 12:
            return_code = sqlite3_exec(db, SELECT_MSG, callback, 0, &errmsg);
            if(return_code != 0){
                error("SQLite died\n");
                return SQLITE_ERROR;
            }
            return 0;
            break;
        default:
            break;
    }
}

void *sender(void *args){
    struct pthread_args *data = (struct pthread_args*)args;
    int send;
    int sock = data->socket;
    char buffer[256];
    unsigned char bystream[sizeof(msg)];
    while(stop) {
        if (stop == 0) break;
        printf("Enter your message: ");
        memset(buffer, 0, 256);
        fgets(buffer, 255, stdin);
        if (stop == 0) break;
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
        if(send > -1){
            if(data->db == NULL){
                error("DB is null\n");
                //return;
            }
            int rc = db_manager(data->db, &msg, 11);
        }
        free(msg.payload);
        if(send < 0) error("ERROR : Writing to socket\n");
    }
}

void *listener(void *args){
    struct pthread_args *data = (struct pthread_args*)args;
    int listen;
    int header[3];
    int sock = data->socket;
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
                if(listen > -1){
                    if(data->db == NULL){
                        error("DB is null\n");
                    }
                    int rc = db_manager(data->db, &recv_data, 11);
                }
                recv_data.payload[recv_data.payload_size] = '\0';
                caesar_decrypt(-13, recv_data.payload);
                printf("Client says: %s\n", recv_data.payload);
                free(recv_data.payload);
            }
        }
        if(listen <= 0) break;
    }
}
