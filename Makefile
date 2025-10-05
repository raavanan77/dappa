CC=gcc
buliddir=./build/
src=./src/

vspf: $(src)vi.c $(src)vspf.c
	$(CC) -o $(buliddir)vspf.o $(src)vspf.c $(src)vi.c
server: $(src)vi.c $(src)server.c
	$(CC) -o $(buliddir)server.o $(src)server.c $(src)vi.c -lsqlite3
client: $(src)vi.c $(src)client.c
	$(CC) -o $(buliddir)client.o $(src)client.c $(src)vi.c -lsqlite3 -lncurses
cripto:
	$(CC) -o $(buliddir)cripto.o $(src)cripto.c -lm
ui:
	$(CC) -o $(buliddir)ui.o $(src)UIwindow.c -lSDL2 -lSDL2_ttf
tui:
	$(CC) -o $(buliddir)tui.o $(src)tui.c -lncurses
clean:
	rm $(buliddir)*.o
