#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "vi.h"

// Constants
#define SMALLCAP 32
#define ALPHALEN 26

// Implementation
void caesar_encrypt(int key, string word)
{
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        char letter = word[i];
        if (isalpha(letter))
        {
            if (isupper(letter))
            {
                char let = letter + key;
                if (let > 'Z')
                {
                    let -= ALPHALEN;
                }
                word[i] = let;
            }
            else if (islower(letter))
            {
                char slet = letter - SMALLCAP;
                slet = slet + key;
                if (slet > 'Z')
                {
                    slet -= ALPHALEN;
                }
                word[i] = slet + SMALLCAP;
            }
        }
    }
}

void caesar_decrypt(int key, string word)
{
    // Decrypt by using negative key
    caesar_encrypt(-key, word);
}

void sub_encrypt(string key, string word)
{
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        char index = 0;
        if (isupper(word[i]))
        {
            index = key[word[i] - 65];
            word[i] = key[index];
        }
        else if (islower(word[i]))
        {
            char small = word[i] - 32;
            index = key[small - 65];
            word[i] = key[index + 32];
        }
    }
}

// Bytestream helper Function
int stream_buffer(unsigned char* buf, struct vi_frame* msg){
    int pos =0;
    memcpy(buf+pos, msg->suuid, 16); pos+= 16;
    memcpy(buf+pos, msg->duuid, 16); pos+= 16;
    *(int *)(buf + pos) = msg->payload_size; pos+= sizeof(int);
    *(time_t *)(buf + pos) = msg->timestamp; pos+= sizeof(time_t);
    memcpy(buf+pos, msg->payload, msg->payload_size);

    return pos + msg->payload_size;
}

int proto_stream_buffer(unsigned char* buf, struct vi_proto* header, int iplen){
    int pos = iplen;
    *(int *)(buf +pos) = header->src; pos += sizeof(uint32_t);
    *(int *)(buf +pos) = header->dest; pos += sizeof(uint32_t);
    *(int *)(buf +pos) = header->nhop; pos += sizeof(uint32_t);
    *(int *)(buf +pos) = header->isdest; pos += 1;

    return 0;
}

int login_buffer(unsigned char* buf, struct usersession* user){
    int pos = 0;
    memcpy(buf+pos, user->name, 16);; pos += 16;
    *(int *)(buf + pos) = user->islooged;

    return pos;

}

int writefile(struct usersession* data){
    FILE* fs = fopen("/home/raavanan/.config/dappa/psm", "w+b");
    printf("FS : %d\n", fs);
    if(fs > 0){
        printf("name :%s", data->name);
        int w = fwrite(data, 1, sizeof(struct usersession), fs);
        if(w > 0) return 0;
    }
    return 1;
}

int readfile(struct usersession* data){
    FILE* fs = fopen("/home/raavanan/.config/dappa/psm", "r");
    int w = fread(data, 1, sizeof(struct usersession), fs);
    printf("Buffers read :%d\n", w);
    if (w > 0){
        return 0;
    }
    return 1;
}