#include <stdio.h>
#include <string.h> // For memcpy

// Define a sample struct
typedef struct {
    int id;
    float value;
    char name[20];
} MyData;

int main() {
    MyData data = {123, 45.67f, "ExampleName"};
    unsigned char byteStream[sizeof(MyData)];

    // Copy the struct's content to the byte stream
    memcpy(byteStream, &data, sizeof(MyData));

    // Print the byte stream (for demonstration)
    printf("Byte stream representation:\n");
    for (size_t i = 0; i < sizeof(MyData); i++) {
        printf("%02X ", byteStream[i]);
    }
    printf("\n");

    // You can now send or store 'byteStream'

    // To convert back from byte stream to struct
    MyData receivedData;
    memcpy(&receivedData, byteStream, sizeof(MyData));

    printf("\nReceived data:\n");
    printf("ID: %d\n", receivedData.id);
    printf("Value: %.2f\n", receivedData.value);
    printf("Name: %s\n", receivedData.name);

    return 0;
}