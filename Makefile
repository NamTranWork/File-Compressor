CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean spotless format

all: encode decode

encode: encode.o node.o stack.o pq.o code.o io.o huffman.o
	$(CC) -o $@ $^

decode: decode.o node.o stack.o pq.o code.o io.o huffman.o
	$(CC) -o $@ $^
	 
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f $(OBJECTS)
	
spotless: clean
	rm -f encode
	rm -f decode

format:
	clang-format -i -style=file encode.c
	clang-format -i -style=file decode.c
	clang-format -i -style=file node.c
	clang-format -i -style=file stack.c
	clang-format -i -style=file pq.c
	clang-format -i -style=file code.c
	clang-format -i -style=file io.c
	clang-format -i -style=file huffman.c
