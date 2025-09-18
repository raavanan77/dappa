#include <stdio.h>
#include <stdlib.h>

struct Node {
    int var;
    struct Node* next;
} arr;

struct Node* createNode(int data){
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));

    if(!newNode){
        return NULL;
    }
    newNode->var = data;
    newNode->next = NULL;
    return newNode;
}

int main(){

}