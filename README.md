# dappa
A simple chat server and multiclient with C socket programming

## Server
Routes messages between clients like a traditional one.

## Client
Able to send, receive and store messages from multiple clients but not on ncurses window.

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

## Build and setup
Install some dependencies that metioned below
 - Fedora - `gcc bulid-essentials ncurses-devel sqlite-devel`
please install respective library according to your linux distro. I'll try on most popular distro later and update this doc.

### Setup
Use `./resource/chat_setup.sh` to setup sql DB and it's schema.
```bash
$:~/dappa#: chmod +x /home/raavanan/dappa/resource/chat_setup.sh
$:~/dappa#: ./resource/chat_setup.sh setup #for the sql stuffs
$:~/dappa#: ./resource/chat_setup.sh wipe 0 or 1 # 0 for wipe server db data and 1 for client
```
### Build
Then build binarys of server and client using Makefile

```bash
$:~/dappa#: make server
$:~/dappa#: make client
```

### Usage
```bash
#For server
$:~/dappa#: ./build/server

#For client 1
$:~/dappa#: ./bulid/client
```

~PS: I haven't made a full user profile register and profiling so we can't use more than 2 client.~

PS Update: I haven't made a full user profile register and profiling but two way comms works but not on ncurses.
I've added two many stupid things and bad memory management that works on hopes and prayers, I'll fix it soon.