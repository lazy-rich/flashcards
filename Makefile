flashcards: main.c set.o media.o card.o
	cc -Wall -Wextra main.c set.o media.o card.o -o flashcards \
		-larchive -lSDL2 -lm -I/usr/include/SDL2

set.o: set.c
	cc -Wall -Wextra -c set.c -o set.o

media.o: media.c
	cc -Wall -Wextra -c media.c -o media.o -I/usr/include/SDL2

card.o: card.c
	cc -Wall -Wextra -c card.c -o card.o -I/usr/include/SDL2

clean:
	rm -f flashcards set.o media.o card.o
