CC=gcc

server:
	$(CC) -o server.o server.c
client:
	$(CC) -o client.o client.c
clean:
	rm *.o
