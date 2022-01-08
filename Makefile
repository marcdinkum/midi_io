CPP = g++
CC = gcc
CFLAGS = -Wall
LDFLAGS= -lportmidi -lpthread -lm -llo


SEQOBJ = midi_io.o sequencer.o
PLAYEROBJ = midi_io.o player.o
RECOBJ = midi_io.o recorder.o
UNCOMPOBJ = uncomposer.o midi_io.o midifile.o
METROOBJ = midi_io.o metronome.o
FILTEROBJ = midi_io.o midifilter.o
BRIDGEOBJ = midi_osc_bridge.o midi_io.o midifile.o

all: sequencer recorder uncomposer metronome midifilter player bridge

sequencer: $(SEQOBJ)
	$(CPP) -o $@ $(CFLAGS) $(SEQOBJ) $(LDFLAGS)

player: $(PLAYEROBJ)
	$(CPP) -o $@ $(CFLAGS) $(PLAYEROBJ) $(LDFLAGS)

recorder: $(RECOBJ)
	$(CPP) -o $@ $(CFLAGS) $(RECOBJ) $(LDFLAGS)

uncomposer: $(UNCOMPOBJ)
	$(CPP) -o $@ $(CFLAGS) $(UNCOMPOBJ) $(LDFLAGS)

bridge: $(BRIDGEOBJ)
	$(CPP) -o $@ $(CFLAGS) $(BRIDGEOBJ) $(LDFLAGS)

metronome: $(METROOBJ)
	$(CPP) -o $@ $(CFLAGS) $(METROOBJ) $(LDFLAGS)

midifilter: $(FILTEROBJ)
	$(CPP) -o $@ $(CFLAGS) $(FILTEROBJ) $(LDFLAGS)

.cpp.o:
	$(CPP) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f `find . -perm /111 -type f`

