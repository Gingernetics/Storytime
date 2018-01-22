all: client server

server: server.o utils.o
	gcc -o server server.o utils.o

client: client.o utils.o
	gcc -o client client.o utils.o

client.o: client.c utils.h
	gcc -c client.c

server.o: server.c utils.h
	gcc -c server.c

utils.o: utils.c utils.h
	gcc -c utils.c

clean:
	rm -f *.o
	rm -f *~
	rm -f client
	rm -f server
