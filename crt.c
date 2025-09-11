#include <stdio.h>
#include <string.h>
#include <math.h>

typedef char* string;

int main(int argc, string argv[]){
    int c ;
    string message = argv[1];
    //fgets(message, 255, stdin);
    int slen = strlen(message);
    printf("Hexed Hash: ");
    int index;
    for(int i= 0; i < slen; i++){
        index = message[i];
        index = ((index % 256) + sin(i)/sqrt(index) * slen);
        printf("%X", index);
    }
    printf("\n");
    return 0;
}
