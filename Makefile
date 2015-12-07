CPP = g++
CC = gcc
CFLAGS = -Wall
LDFLAGS= -L/usr/local/lib -lportmidi -lpthread -lm


#MACLIB = -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

SEQOBJ = midi_io.o sequencer.o
RECOBJ = midi_io.o recorder.o
UNCOMPOBJ = uncomposer.o midi_io.o midifile.o
METROOBJ = midi_io.o metronome.o
FILTEROBJ = midi_io.o midifilter.o

all: sequencer recorder uncomposer metronome midifilter

sequencer: $(SEQOBJ)
	$(CPP) -o $@ $(CFLAGS) $(SEQOBJ) $(LDFLAGS)

recorder: $(RECOBJ)
	$(CPP) -o $@ $(CFLAGS) $(RECOBJ) $(LDFLAGS)

uncomposer: $(UNCOMPOBJ)
	$(CPP) -o $@ $(CFLAGS) $(UNCOMPOBJ) $(LDFLAGS)

metronome: $(METROOBJ)
	$(CPP) -o $@ $(CFLAGS) $(METROOBJ) $(LDFLAGS)

midifilter: $(FILTEROBJ)
	$(CPP) -o $@ $(CFLAGS) $(FILTEROBJ) $(LDFLAGS)

.cpp.o:
	$(CPP) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm +111 -type f`

