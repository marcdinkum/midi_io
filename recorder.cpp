/**********************************************************************
*          Copyright (c) 2021, Hogeschool voor de Kunsten Utrecht
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
*  File name     : recorder.cpp
*  System name   : MIDI I/O
* 
*  Description   : Demo program for wrapper class for portmidi
*
*
*  Author        : Marc_G
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#include <iostream>
#include <vector>
#include "midi_io.h"


int main(int argc, char **argv)
{
MIDI_io midi_io;
PmEvent event;
vector<PmEvent> eventlist;
vector<PmEvent>::iterator event_iterator;
bool event_read;
int input_device=0,output_device=0;
bool use_default_devices=false;
bool recording=false;
bool looping=false;
unsigned char cmd,channel,data1;

  midi_io.list_devices();

  if(argc>1 && (string(argv[1]) == "-d")) {
    use_default_devices=true;
    std::cout << "Using default devices\n";
  }
  else std::cout << "For using default devices specify -d\n";

  if(!use_default_devices){
    std::cout << "\nGive input device number: ";
    cin >> input_device;
    midi_io.set_input_device(input_device);
    std::cout << "Give output device number: ";
    cin >> output_device;
    midi_io.set_output_device(output_device);
  }

  midi_io.initialise();

  // reset filters: accept all events
  midi_io.set_input_filter(0);

  std::cout << "\nUse MIDI keys play, rec, stop, rewind and loop\n";
/*
  play	b0 f 75 7f
  rec	b0 f 76 7f
  stop	b0 f 74 7f
  rewind	b0 f 72 7f
  loop	b0 f 71 7f
*/

  while(true)
  {
    event_read = midi_io.read_event(event);
    if(event_read){
      //midi_io.write_event(&event); // copy to output
      cmd=Pm_MessageStatus(event.message)&0xf0;
      channel=Pm_MessageStatus(event.message)&0xf;
      data1=Pm_MessageData1(event.message);
      std::cout << (hex) << (int) cmd << " " << (int) channel << " " << (int)
              data1 << " " << (int) data1 << endl;
      // only store note_on and note_off
      if(recording && (cmd == 0x90 || cmd == 0x80)) eventlist.push_back(event);
      if(cmd & 0x80) {
        std::cout << "Now recording\n";
	recording=true;
        midi_io.reset_timebase();
      }
      if(cmd == 0xb0 && data1 == 0x76) {
        std::cout << "Now recording\n";
	recording=true;
        midi_io.reset_timebase();
      }
      if(cmd == 0xb0 && data1 == 0x75) break;
      if(cmd == 0xb0 && data1 == 0x74) break;
      if(cmd == 0xb0 && data1 == 0x71){
        looping=true;
        break;
      }
    }
    else usleep(10000);
  } // while

  /*
   * Play back what we've just captured
   */
  std::cout << "Now playing\n";
  do {
    event_iterator=eventlist.begin();
    midi_io.reset_timebase();
    while(event_iterator != eventlist.end()){
      if(midi_io.get_currenttime() >= event_iterator->timestamp){
	// It's time to play
	event = *event_iterator;
	midi_io.write_event(&event);
	event_iterator->timestamp*=0.9;
	event_iterator++;
      } // if
	else usleep(10000);
    } // while
  } while(looping);

  midi_io.finalise();
  return 0;
}

