CC=gcc
#CC=arm-linux-gcc
server:server.o
	$(CC) -o server	server.o -lpthread
server.o:server.c
	$(CC) -c server.c 
clean:
	rm -rf *.o
