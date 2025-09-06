#include <stdio.h>
#include <pthread.h>

void *thread_listener(){
    printf("Hello there \n");
}

int main(int argc, char* argv[]){
    pthread_t thread;
    printf("Hello there first time \n");
    pthread_create(&thread, NULL, thread_listener, NULL);
    pthread_join(thread, NULL);
    return 0;
}