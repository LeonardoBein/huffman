############################# Makefile ##########################
all: huffman
huffman: function.o main.o
	gcc -o huffman function.o main.o
fuction.o: function.c header.h
	gcc -c function.c
main.o: main.c header.h
	gcc -c main.c
clean:
	rm -rf *.o huffman
