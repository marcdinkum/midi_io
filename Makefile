CPP = g++
CC = gcc
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportmidi -lpthread -lm


#MACLIB = -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

OBJ = midi_io.o sequencer.o

all: sequencer

sequencer: $(OBJ)
	$(CPP) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)


.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm +111 -type f`

