CC=gcc
DEPS = costs.h receiver.h
CFLAGS = -Wall -std=c99 -pthread -Wno-missing-braces
OBJ = main.o costs.o receiver.o updater.o sender.o logger.o machine.o

default: main

# linking
main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

# compiling
main.o: main.c receiver.h updater.h machine.h
	$(CC) $(CFLAGS) -c main.c

costs.o: costs.h logger.h
logger.o: logger.h
machine.o: machine.h costs.h
receiver.o: receiver.h logger.h costs.h sender.h
sender.o: costs.h sender.h machine.h logger.h
updater.o: updater.h util.h logger.h
util.o: util.h

clean:
	-rm -f *.o
	-rm main
