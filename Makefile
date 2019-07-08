HOME    = $(PWD)
CC      = gcc-7
CFLAGS  = -Wall -Wextra -pedantic -O3 -march=skylake -fopenmp
UNAME 	:= $(shell uname)


all:
	$(CC) $(CFLAGS) -o main main.c grayscale.c

clean:
	rm -f *.o main 