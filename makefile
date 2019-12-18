build:
	gcc main.c interpret.c -o main.o -lSDL2 -lSDL2_ttf

run:
	./main.o

test:
	make build
	make run
