#ifndef VI_H
#define VI_H

#include "vi.h"
#include <stdint.h>
#include <stdbool.h>

#define INSERT_MSG "INSERT INTO messages VALUES (?, ?, ?, ?)"
#define INSERT_USER "INSERT INTO userlist(username, leasetime, dormant) VALUES (?, ?, ?)"
#define INSERT_SESSION "INSERT INTO session VALUES (?, ?)"
#define DELETE_MSG "DELETE from messages where message = ?"
#define SELECT_MSG "SELECT * FROM messages"
#define FIND_USER "SELECT * FROM userlist WHERE username = ?"
#define GET_USER_LOGIN "SELECT * FROM session WHERE TRIM(user) = ?"

typedef char* string;

struct userlist {
    int uuid;
    char username[16];
    int leasetime;
    bool dormant;
};

struct usersession {
    char name[16];
    int islooged;
};

struct vi_frame {
    char suuid[16];
    char duuid[16];
    int payload_size;
    time_t timestamp;
    string payload;
};

struct vi_proto {
    uint32_t src;
    uint32_t dest;
    uint32_t nhop;
    bool isdest;
};

void caesar_encrypt(int key, string word);

void caesar_decrypt(int key, string word);

int stream_buffer(unsigned char* buf, struct vi_frame* msg);

int proto_stream_buffer(unsigned char* buf, struct vi_proto* header, int iplen);

int login_buffer(unsigned char* buf, struct usersession* user);

int writefile(struct usersession* data);

int readfile(struct usersession* data);
#endif // VI_H
