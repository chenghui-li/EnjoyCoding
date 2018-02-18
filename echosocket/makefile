all : server client myIO.o server.o client.o

server : myIO.o server.o
	gcc -o server myIO.o server.o
client : myIO.o client.o
	gcc -o client myIO.o client.o
myIO.o:myIO.c myIO.h mysocket.h
	gcc -c myIO.c
server.o:server.c myIO.h mysocket.h
	gcc -c server.c -g
client.o : client.c myIO.h mysocket.h
	gcc -c client.c -g
clean:
	rm all server client myIO.o server.o client.o
