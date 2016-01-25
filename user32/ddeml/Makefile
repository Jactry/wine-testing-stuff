default: all

dde_client.exe: dde_client.c
	i686-w64-mingw32-gcc dde_client.c -o dde_client.exe -Wall

dde_server.exe: dde_server.c
	i686-w64-mingw32-gcc dde_server.c -o dde_server.exe -Wall -I../../include

all: dde_client.exe dde_server.exe

clean:
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*~" -exec rm -f {} \;
	find . -name "*.exe" -exec rm -f {} \;
