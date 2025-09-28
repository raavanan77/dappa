#ifndef VI_H
#define VI_H

#include "vi.h"
#include <stdint.h>
#include <stdbool.h>

#define INSERT_MSG "INSERT INTO messages VALUES (?, ?, ?, ?)"
#define DELETE_MSG "DELETE from messages where message = ?"
#define SELECT_MSG "SELECT * FROM messages"

typedef char* string;

struct vi_frame {
    int suuid;
    int duuid;
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

#endif // VI_H
