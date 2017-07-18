VERSION = 0.0.1
CC = gcc
OBJS = ctronome.o dsp.o

all: ctronome

%.o: %.c ctronome.h
	$(CC) -c $<

ctronome: $(OBJS)
	$(CC) -o ctronome $(OBJS)

clean:
	rm -f *.o ctronome

