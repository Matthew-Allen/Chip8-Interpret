build:
	gcc main.c -o main.o

run:
	./main.o

test:
	make build
	make run
