#ifndef VI_H
#define VI_H

typedef char* string;

struct vi_frame {
  int suuid;
  int duuid;
  int payload_size;
  string payload;
};

int stream_buffer(unsigned char* buf, struct vi_frame* msg){
    int pos =0;
    *(int *)(buf + pos) = msg->suuid; pos+= sizeof(int);
    *(int *)(buf + pos) = msg->duuid; pos+= sizeof(int);
    *(int *)(buf + pos) = msg->payload_size; pos+= sizeof(int);
    memcpy(buf+pos, msg->payload, msg->payload_size);

    return pos + msg->payload_size;
}

#endif // VI_H
