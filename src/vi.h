#ifndef VI_H
#define VI_H

#include "vi.h"
#include <stdint.h>
#include <stdbool.h>

typedef char* string;

struct vi_frame {
    int suuid;
    int duuid;
    int payload_size;
    time_t timestamp;
    string payload;
};

struct vi_proto {
    uint8_t src;
    uint8_t dest;
    uint8_t nhop;
    bool isdest;
};

void caesar_encrypt(int key, string word);

void caesar_decrypt(int key, string word);

int stream_buffer(unsigned char* buf, struct vi_frame* msg);

#endif // VI_H
