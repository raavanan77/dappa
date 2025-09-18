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

## Things currently working on
- Database for store messages
- Dynamic list of connections
- User register and profile

## Build
```bash
$: make server
$: make client
```

## Usage
```bash
#For server
$: ./server

#For client 1
$: ./client

#For client 2
$: ./client 127.0.0.1 7733
```

PS: I haven't made a full user profile register and profiling so we can't use more than 2 client.