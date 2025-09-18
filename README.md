# dappa
A simple chat server and multiclient with C socket programming

## Server
Routes messages between clients like a traditional one

## Client
Able to send, receive and store messages from multiple clients

## Custom headers 
- Vi_frame -> Headers for the message

```c
struct vi_frame {
    int suuid;
    int duuid;
    int payload_size;
    time_t timestamp;
    string payload;
};
```