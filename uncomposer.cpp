/**********************************************************************
*                Copyright (c) 2015, Marc Groenewegen
*                      Utrecht, the Netherlands
*                          All rights reserved
***********************************************************************
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.
*  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************
*
*  File name     : uncomposer.cpp
*  System name   : MIDI I/O
* 
*  Description   : Capture MIDI input and store in a MIDI file
*
*
*  Author        : Marc_G
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include "midi_io.h"
#include "midifile.h"



/*
  TODO

  - metronome and time signature
  - ports specification on command line
  - program change, pitch bend etc. ook opslaan

*/

int main(int argc, char **argv)
{
MIDI_io midi_io; // PortMidi wrapper instance
PmEvent event; // PortMidi event
MidiFile midifile;
char midiEvent[3];
std::string filename="uncomposer.midi";
bool event_read;
int input_device=0;
bool use_default_devices=false;
unsigned long prev_timestamp=0,new_timestamp=0,delay=0;
unsigned char cmd,data1,data2;

  midi_io.list_devices();

  midifile.set_bpm(120);

  if(argc>1) filename=argv[1];
  midifile.open(filename.c_str());

  if(!use_default_devices){
    std::cout << "\nGive input device number: ";
    cin >> input_device;
    midi_io.set_input_device(input_device);
  }

  midi_io.initialise();

  std::cout << std::endl << std::endl << "Writing output to " << filename <<
    " after receiving lower E or higher E" << std::endl << std::endl;

  // reset filters: accept all events
  midi_io.set_input_filter(PM_FILT_REALTIME|PM_FILT_PROGRAM|PM_FILT_PITCHBEND|PM_FILT_AFTERTOUCH);

  midifile.write_header();
  midifile.start_track();

  while(true)
  {
    event_read = midi_io.read_event(event);
    if(event_read){
      cmd=Pm_MessageStatus(event.message);
      data1=Pm_MessageData1(event.message);
      data2=Pm_MessageData2(event.message);
      std::cout << (hex) << (int) cmd << " " <<
        (int) data1 << " " << (int) data1 << std::endl;
      // only store note_on and note_off
      if((cmd&0xf0)==0x90 && data1==28){
        event_read = midi_io.read_event(event); // fetch the note off
        usleep(2000);
        break; // magic note to end the recording
      }
      if((cmd&0xf0) == 0x90 || (cmd&0xf0) == 0x80){
         midiEvent[0]=cmd;
         midiEvent[1]=data1;
         midiEvent[2]=data2;
         new_timestamp=midi_io.get_currenttime();
	 delay=new_timestamp-prev_timestamp;
	 midifile.write_event(delay*10,midiEvent,3); // Why *10 ??
	 prev_timestamp=new_timestamp;
      } // if note on/off
      if((cmd&0xf0) == 0xb0){ // control change
         midiEvent[0]=cmd;
         midiEvent[1]=data1;
         midiEvent[2]=data2;
         new_timestamp=midi_io.get_currenttime();
	 delay=new_timestamp-prev_timestamp;
	 midifile.write_event(delay*10,midiEvent,3); // Why *10 ??
	 prev_timestamp=new_timestamp;
      }
    }
    else usleep(2000);
  } // while

  midifile.end_track();
  midifile.close();

  midi_io.finalise();

  return 0;
}

