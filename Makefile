VERSION = 0.5.3
CC = gcc
OBJS = ctronome.o dsp.o routines.o

all: ctronome

%.o: %.c ctronome.h routines.h
	$(CC) -c $<

ctronome: $(OBJS)
	$(CC) -o ctronome $(OBJS)

clean:
	rm -f *.o ctronome

