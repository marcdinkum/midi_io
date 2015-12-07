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
#include <vector>
#include "midi_io.h"
#include "midifile.h"


using namespace std;

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
char *filename="test.midi";
bool event_read;
int input_device=0;
bool use_default_devices=false;
unsigned long prev_timestamp=0,new_timestamp=0,delay=0;
unsigned char cmd,channel,data1,data2;

  midi_io.list_devices();

  midifile.set_bpm(120);

  if(argc>1) filename=argv[1];
  midifile.open(filename);

  if(!use_default_devices){
    cout << "\nGive input device number: ";
    cin >> input_device;
    midi_io.set_input_device(input_device);
  }

  midi_io.initialise();

  cout << endl << endl << "Writing output to " << filename <<
    " after receiving lower E or higher E" << endl << endl;

  // reset filters: accept all events
  midi_io.set_input_filter(0);

  midifile.write_header();
  midifile.start_track();

  while(true)
  {
    event_read = midi_io.read_event(event);
    if(event_read){
      cmd=Pm_MessageStatus(event.message)&0xf0;
      channel=Pm_MessageStatus(event.message)&0xf;
      data1=Pm_MessageData1(event.message);
      data2=Pm_MessageData2(event.message);
      cout << (hex) << (int) cmd << " " << (int) channel << " " << (int)
              data1 << " " << (int) data1 << endl;
      // only store note_on and note_off
      if(data1==28 || data1==100) break; // magic notes to end the recording
      if(cmd == 0x90 || cmd == 0x80){
         midiEvent[0]=cmd|channel;
         midiEvent[1]=data1;
         midiEvent[2]=data2;
         new_timestamp=midi_io.get_currenttime();
	 delay=new_timestamp-prev_timestamp;
	 midifile.write_event(delay*10,midiEvent,3); // Why *10 ??
	 prev_timestamp=new_timestamp;
      } // if note on/off
    }
    else usleep(2000);
  } // while

  midifile.end_track();
  midifile.close();

  midi_io.finalise();

  return 0;
}

