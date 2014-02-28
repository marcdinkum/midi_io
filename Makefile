CPP = g++
CC = gcc
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportmidi -lpthread -lm


#MACLIB = -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

SEQOBJ = midi_io.o sequencer.o
RECOBJ = midi_io.o recorder.o
METROOBJ = midi_io.o metronome.o

all: sequencer recorder metronome

sequencer: $(SEQOBJ)
	$(CPP) -o $@ $(CFLAGS) $(SEQOBJ) $(LDFLAGS)

recorder: $(RECOBJ)
	$(CPP) -o $@ $(CFLAGS) $(RECOBJ) $(LDFLAGS)

metronome: $(METROOBJ)
	$(CPP) -o $@ $(CFLAGS) $(METROOBJ) $(LDFLAGS)


.cpp.o:
	$(CPP) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm +111 -type f`

