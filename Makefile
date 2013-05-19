CPP = g++
CC = gcc
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportmidi -lpthread -lm


#MACLIB = -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

SEQOBJ = midi_io.o sequencer.o
RECOBJ = midi_io.o recorder.o

all: sequencer recorder

sequencer: $(SEQOBJ)
	$(CPP) -o $@ $(CFLAGS) $(SEQOBJ) $(LDFLAGS)

recorder: $(RECOBJ)
	$(CPP) -o $@ $(CFLAGS) $(RECOBJ) $(LDFLAGS)


.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm +111 -type f`

