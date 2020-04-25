flashcards: main.c set.o media.o
	cc -Wall -Wextra main.c set.o media.o -o flashcards \
		-larchive -lSDL2 -lm -I/usr/include/SDL2
set.o: set.c
	cc -Wall -Wextra -c set.c -o set.o

media.o: media.c
	cc -Wall -Wextra -c media.c -o media.o -I/usr/include/SDL2

clean:
	rm -f flashcards set.o
