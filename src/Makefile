CC=clang
CPP=clang++

all: tinydbg hello

linenoise.o: linenoise.h linenoise.c
	$(CC) -g -c -O0 linenoise.c

tinydbg: main.cpp linenoise.o
	$(CPP) main.cpp -g -O0 linenoise.o -lelf++ -ldwarf++ -Wall -o tinydbg

hello: hello.c
	$(CC) hello.c -g -O0 -Wall -o hello

clean:
	rm -f tinydbg hello *.o
