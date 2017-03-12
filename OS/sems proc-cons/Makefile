CFLAGS=-std=c99 -Wall

all: server consumer producer killer

server: func.o
	gcc $(CFLAGS) $^ server.c -o server.out
	
producer: func.o
	gcc $(CFLAGS) $^ producer.c -o prod.out
	
consumer: func.o
	gcc $(CFLAGS) $^ client.c -o cons.out
	
killer: func.o
	gcc $(CFLAGS) $^ killer.c -o kill.out
	
func.o:
	gcc $(CFLAGS) func.c -c
