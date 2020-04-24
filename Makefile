flashcards: main.c set.o
	cc -Wall -Wextra main.c set.o -o flashcards -larchive

set.o: set.c
	cc -Wall -Wextra -c set.c -o set.o

clean:
	rm -f flashcards set.o
