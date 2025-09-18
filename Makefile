CC=gcc
buliddir=./build/
src=./src/

vspf: $(src)vi.c $(src)vspf.c
	$(CC) -o $(buliddir)vspf.o $(src)vspf.c $(src)vi.c
server: $(src)vi.c $(src)server.c
	$(CC) -o $(buliddir)server.o $(src)server.c $(src)vi.c 
client: $(src)vi.c $(src)client.c
	$(CC) -o $(buliddir)client.o $(src)client.c $(src)vi.c
cripto:
	$(CC) -o $(buliddir)cripto.o $(src)cripto.c -lm
ui:
	$(CC) -o $(buliddir)ui.o $(src)UIwindow.c -lSDL2 -lSDL2_ttf
clean:
	rm $(buliddir)*.o
