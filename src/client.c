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
#include <poll.h>
#include <sqlite3.h>
#include <ncurses.h>
#include "vi.h"

static volatile int stop = 1;
struct vi_frame msg, recv_data, msglist;
struct tm *ltime;
int start_y = 0 , start_x = 0;
int chat_cursor = 2;

struct pthread_args{
    struct pollfd *fd;
    sqlite3 *db;
};

void killserver();
void error(const char *msg);
void *listener(void *nsock);
void *sender(void *nsock);
static int callback(void *NU, int argc, char **argv, char **azColName);
sqlite3 *dbpointer();
int post_clean_up(sqlite3 *db);
int db_manager(sqlite3 *db, void *data, int method);
void exit_curse();
int text_inputer(string data);
void cursor_block();
void cursor_underline();
void cursor_beam();

int main(int argc, char *argv[]){
    //socket vars
    int sockfd, portno, n;
    string hostname;
    struct pollfd fds[1];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t listen_t, sender_t;
    char buffer[256];
    sqlite3 *db = dbpointer();
    struct pthread_args args;
    struct usersession login;
    char name[16];

    if(db == NULL){
        error("DB is null\n");
        return EXIT_FAILURE;
    }
    
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
    fds[0].fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fds[0].fd < 0) error("ERROR: Opening socket\n");
    server = gethostbyname(hostname);
    if(server == NULL){
        fprintf(stderr, "ERROR: No such host\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((string) server->h_addr_list[0],(string)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(fds[0].fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR: Connecting...");
    fds[0].events = POLLIN;
    int flag;
    int isloggedin = readfile(&login);
    unsigned char uname[sizeof(login)];
    int sizeu;
    if(isloggedin == 1){
        printf("Please login with your username :\n");
        fgets(name, 15, stdin);
        name[strcspn(name, "\n")] = '\0';
        login.islooged = 1;
        memcpy(login.name, name, 16);
        login.name[15] = '\0';
        printf("Name :%s, logname :%s\n", name, login.name);
        sizeu = login_buffer(uname, &login);
        int send = write(fds[0].fd, uname, sizeu);
        printf("sent :%d\n", send);
        if(send > 0) { 
            fsync(sockfd);  // Ensure data is sent
            usleep(100000);  // 100ms delay
            //if(fds[0].revents & POLLIN){   
                send = read(fds[0].fd, &flag, sizeof(int));
                printf("Flag :%d\n", flag);
                if(send > 0 && flag == 0){
                    isloggedin = writefile(&login);
                    if(isloggedin != 0){
                        return 22;
                    }
                    strcpy(msg.suuid, login.name);
                    printf("SUUID set to %s\n", msg.suuid);
                }
                else {
                    isloggedin = writefile(&login);
                    strncpy(msg.suuid, name, 16);
                    printf("SUUID set to %s\n", msg.suuid);
                }
            //}
        }
    }
    else {
        sizeu = login_buffer(uname, &login);
        int send = write(fds[0].fd, uname, sizeu);
        if(send > 0) { 
            fsync(sockfd);  // Ensure data is sent
            usleep(100000);  // 100ms delay
            //if(fds[0].revents & POLLIN){   
                send = read(fds[0].fd, &flag, sizeof(int));
                printf("Flag :%d\n", flag);
                if(send > 0 && flag == 0){
                    isloggedin = writefile(&login);
                    if(isloggedin != 0){
                        return 22;
                    }
                    strcpy(msg.suuid, login.name);
                    printf("SUUID set to %s\n", msg.suuid);
                }
                else {
                    isloggedin = writefile(&login);
                    strncpy(msg.suuid, login.name, 16);
                    printf("SUUID set to %s\n", msg.suuid);
                }
            //}
        }
    }
    signal(SIGINT, killserver);
    args.fd = fds;
    args.db = db;
    pthread_create(&listen_t, NULL, listener, &args);
    pthread_create(&sender_t, NULL, sender, &args);
    pthread_join(listen_t, NULL);
    pthread_join(sender_t, NULL);
    post_clean_up(db);
    close(fds[0].fd);
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

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    struct usersession* user = (struct usersession*)data;
    if(argc > 0){
        strcpy(user->name,argv[0]);
        user->islooged = atoi(argv[1]);
        printf("User :%s islogged :%d\n", user->name, user->islooged);
        return 0;
    }
    else{
        return 1;
    }
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

int db_manager(sqlite3 *db, void *data , int method){
    /*
    method code
    09 - set loggin session
    10 - Check login
    11 - INSERT
    12 - SELECT
    13 - DELETE
    14 - UPDATE
    */
   
   // SQLite vars
   int return_code;
   string errmsg = 0;
   sqlite3_stmt *stmt;
   struct vi_frame* message;
   struct usersession* ses;
   if(method > 10){
       message = (struct vi_frame*)data;
    }
    else if(method < 11){
        ses = (struct usersession*)data;
    }
    switch (method){
        case 9:
            sqlite3_prepare_v2(db, INSERT_SESSION, -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, ses->name, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, ses->islooged);
            return_code = sqlite3_step(stmt);
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                return SQLITE_ERROR;
            }
            sqlite3_finalize(stmt);
            return 0;
            break;
        case 10:
            sqlite3_prepare_v2(db, "SELECT * FROM session LIMIT 1", -1, &stmt, NULL);
            //sqlite3_bind_text(stmt, 1, ses->name, -1, SQLITE_STATIC);
            while((return_code == sqlite3_step(stmt)) == SQLITE_ROW){
                const string usernamedb = (const string)sqlite3_column_text(stmt, 0);
                strncpy(ses->name, usernamedb, 16);
                ses->islooged = sqlite3_column_int(stmt, 1);
            }
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                return SQLITE_ERROR;
            }
            else if(ses->islooged == 1){
                return 0;
            }
            sqlite3_finalize(stmt);
            return 1;
            break;
        case 11:
            sqlite3_prepare_v2(db, INSERT_MSG, -1, &stmt, NULL);
            if(strcmp(msg.suuid, message->suuid) == 0){
                sqlite3_bind_int(stmt, 1, 1);
                sqlite3_bind_int(stmt, 2, 0);
            }
            else{
                sqlite3_bind_int(stmt, 1, 0);
                sqlite3_bind_int(stmt, 2, 1);
            }
            sqlite3_bind_text(stmt, 3, message->payload, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 4, message->timestamp);
            return_code = sqlite3_step(stmt);   
            if(return_code != SQLITE_DONE){
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt);
                perror("Login failed. Exiting method > 10 message...");
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

void *listener(void *args){
    struct pthread_args *data = (struct pthread_args*)args;
    int listen;
    int header;
    int sock = data->fd[0].fd;
    while(stop) {
        if (stop == 0) break;
        if(data->fd[0].revents & POLLIN){
            time_t timestamp;
            unsigned char buffered[sizeof(recv_data)];
            listen = read(sock, recv_data.suuid, 16);
            listen = read(sock, recv_data.duuid, 16);
            if(listen > 0){
                recv_data.payload_size = header;
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
                    printf("Client says Cyphered text: %s\n", recv_data.payload);
                    caesar_decrypt(-13, recv_data.payload);
                    printf("Client says: %s\n", recv_data.payload);
                    free(recv_data.payload);
                }
            }
            if(listen <= 0) break;
        }
    }
    close(sock);
}

int text_inputer(string chatinput){
    int pos = 0;
  char c;
  while(pos < 256){
    c = getch();
    if(c == 27 || c == '\n'){
      noecho();
      break;
    }
    chatinput[pos++] = c;
  }
  chatinput[pos] = '\0';
  return 0;
}

// Block cursor (insert mode style)
void cursor_block() {
    printf("\033[2 q");
    fflush(stdout);
}

// Underline cursor (normal mode style)
void cursor_underline() {
    printf("\033[4 q");
    fflush(stdout);
}

// Beam/line cursor
void cursor_beam() {
    printf("\033[6 q");
    fflush(stdout);
}

void *sender(void *args){
    struct pthread_args *data = (struct pthread_args*)args;
    int send;
    int sock = data->fd[0].fd;
    char buffer[256];
    unsigned char bystream[sizeof(msg)];
    printf("Set receivers uuid :\n");
    initscr();
    cbreak();
    keypad(stdscr, true);
    noecho();
    getyx(stdscr, start_y, start_x);
    WINDOW* chat_list = newwin(LINES - 1, (int)COLS * 20 / 100, start_y, start_x);
    WINDOW* chat_search = newwin((int)LINES * 7 / 100 - 1, ((int)COLS * 20 / 100) - 2,  (int)LINES * 5 / 100 - 1, (int)COLS * 1 / 100); //  Don't think I can finish this feature anytime soon
    WINDOW* text_input_box = newwin((int)LINES * 7 / 100 , ((int)COLS * 80 / 100),  (int)LINES * 93 / 100 , (int)COLS * 20 / 100);
    WINDOW* command = newwin(((int)LINES * 5 / 100) + 2, COLS, LINES - 1, 0);
    refresh();
    box(chat_list, 0, 0);
    mvwprintw(chat_list, 1, 1, "Chat");
    wrefresh(chat_list);
    refresh();
    box(text_input_box, 0, 0);
    wrefresh(text_input_box);
    //signal(SIGINT, exit_curse);
    while(stop){
        int mode = getch();
        switch (mode){
        case 'f':
            echo();
            box(chat_search, 0, 0);
            wrefresh(chat_search);
            char c;
            move(3, chat_cursor);
            text_inputer(buffer);
            noecho();
            break;
        case 'r':
            wclear(text_input_box);
            box(text_input_box, 0, 0);
            wrefresh(text_input_box);

        case 't':
            echo();
            move(((int)LINES * 95 / 100) + 1 , ((int)COLS * 20 / 100)+1);
            //memset(buffer, 0, 256);
            int rc = text_inputer(buffer);
            //fgets(buffer, 255, stdin);
            if (stop == 0) break;
            buffer[strcspn(buffer, "\n")] = '\0';
            mvwprintw(chat_list, 3, 3, buffer);
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
            if(rc == 0){
                wrefresh(text_input_box);
            }
            noecho();
            break;
        case ':':
            echo();
            string comms;
            move(LINES - 1, 0);
            int rcw = text_inputer(comms);
            if(strcmp(comms, "/logout") == 0){
                killserver();
            }
            if(rcw == 0){
                wclear(command);
                mvwprintw(command, 1, 1, comms);
                wrefresh(command);
            }
            noecho();
            break;
        case KEY_RESIZE:
            getyx(stdscr, start_y, start_x);
            refresh();
            wresize(chat_list, start_y, start_x);
            wrefresh(chat_list);
            wrefresh(chat_search);
            wrefresh(text_input_box);
            break;
        case 27:
            stop = 0;
            break;
        default:
            noecho();
            break;
        }
        if (stop == 0) break;
        //printf("Enter your message: ");
    }
    close(sock);
    endwin();
}