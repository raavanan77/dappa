#include <stdio.h>
#include <stdlib.h>

struct Node {
    int var;
    struct Node* next;
} arr;

struct Node* createNode(int data){
    struct Node* newNode = malloc(sizeof(struct Node));

    if(!newNode){
        return NULL;
    }
    newNode->var = data;
    newNode->next = NULL;
    return newNode;
}

void insert(struct Node** head, int data){
    struct Node* newNode = createNode(data);

    if(*head == NULL){
        *head = newNode;
        return;
    }
    
    struct Node* current = *head;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = newNode;
}

void printNode(struct Node* head){
    struct Node* current = head;
    while(current != NULL){
        printf("%d -> ", current->var);
        current = current->next;
    }
    printf("End\n");
}

void deleteNode(struct Node* head, int data){
    struct Node* current = head;
    struct Node* prev;
    while(current->var != data){
        prev = current;
        current = current->next;
    }
    // if(current->next != NULL) {
    //     prev->next = current->next;
    // }
    // else{
    //     prev->next = NULL;
    // }
    printf("value to delete:%d\n", current->var);
}

int main(){
    int input;
    struct Node* head = NULL;
    while(1){
        scanf("%d", &input);
        if(input == 0) break;
        insert(&head, input);
        printNode(head);
    }
    deleteNode(head, 10);
    printNode(head);
    return 0;
}