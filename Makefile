CC = gcc
CFLAGS = -Wall -std=c99

hash: hash.o sha256.o sha256constants.o
	gcc hash.o sha256.o sha256constants.o -o hash

hash.o: hash.c sha256.h
	gcc -Wall -std=c99 -g -c -g -o hash.o hash.c

sha256.o: sha256.c sha256.h sha256constants.h
	gcc -Wall -std=c99 -g -c -o sha256.o sha256.c

sha256constants.o: sha256constants.c sha256constants.h
	gcc -Wall -std=c99 -g -c -o sha256constants.o sha256constants.c

sha256test: sha256test.o sha256.o sha256constants.o
	gcc sha256test.o sha256.o sha256constants.o -o sha256test

sha256test.o: sha256test.c sha256.h
	gcc -Wall -std=c99 -g -c -o sha256test.o sha256test.c

clean:
	rm -f hash.o
	rm -f sha256.o
	rm -f sha256constants.o
	rm -f sha256test.o
	rm -f hash
	rm -f sha256test
	rm -f output.txt
	rm -f stdout.txt
	rm -f stderr.txt