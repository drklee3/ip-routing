CC=gcc
DEPS = costs.h receiver.h
CFLAGS = -Wall -std=c99 -pthread -Wno-missing-braces

default: main

# linking
main: main.o costs.o receiver.o updater.o sender.o logger.o machine.o
	$(CC) $(CFLAGS) -o main main.o costs.o receiver.o updater.o sender.o logger.o machine.o

# compiling
main.o: main.c receiver.h updater.h machine.h
	$(CC) $(CFLAGS) -c main.c

costs.o: costs.h logger.h
logger.o: logger.h
machine.o: machine.h costs.h
receiver.o: receiver.h logger.h costs.h
sender.o: sender.h machine.h logger.h
updater.o: updater.h util.h logger.h
util.o: util.h

clean:
	-rm -f *.o
	-rm main
