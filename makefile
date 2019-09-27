build:
	gcc main.c screen.c -o main.o -lSDL2

run:
	./main.o

test:
	make build
	make run
