CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

all: main

main: main.o hetero_list.o type_detect.o
	$(CC) $(CFLAGS) -o main main.o hetero_list.o type_detect.o

main.o: main.c hetero_list.h type_detect.h
	$(CC) $(CFLAGS) -c main.c

hetero_list.o: hetero_list.c hetero_list.h type_detect.h
	$(CC) $(CFLAGS) -c hetero_list.c

type_detect.o: type_detect.c type_detect.h
	$(CC) $(CFLAGS) -c type_detect.c

clean:
	rm -f *.o main
