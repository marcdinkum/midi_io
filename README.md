Easy-to-use C++ abstraction of the PortMidi C library.

This code uses blocking I/O instead of the default callback method as this
is easier to explain to beginning programmers.

PortMidi is a cross-platform C library for MIDI IO
created by Roger B. Dannenberg
https://github.com/PortMidi/portmidi

---

Example programs based on said C++ library, mainly for educational purposes
and some are quite useful :-)

midifilter: let MIDI pass from input to output while filtering out unwanted MIDI messages

player: play a couple of MIDI messages to test your output

recorder: records a bit of MIDI and plays it back afterwards

uncomposer: records MIDI data as long as you don't hit a designated hardcoded note to signal you're done, then writes everything to a MIDI file containing the date and time

bridge: MIDI to OSC bridge (Open Sound Control, needs liblo)

sequencer: simple MIDI sequencer

metronome: a metronome with some optional parameters
    [bpm] [bpm_increment] [beats-per-bar] [accent-note] [default-note]


# Virtual devices
As of 2021 portmidi offers the possibility of creating virtual devices. Or
to be more precise: named ports behaving like devices.

Use cases
- dynamically create MIDI ports within your own program, optionally to be
  connected to by other applications as they become available
- testing without the need for external devices

