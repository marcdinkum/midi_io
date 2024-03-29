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
*  File name     : sequencer.cpp
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

  midi_io.create_virtual_input_device("sequencer");
  midi_io.create_virtual_output_device("sequencer");

  midi_io.list_devices();

  std::cout << "\nGive input device number: ";
  cin >> input_device;
  midi_io.set_input_device(input_device);
  std::cout << "Give output device number: ";
  cin >> output_device;
  midi_io.set_output_device(output_device);

  midi_io.initialise();

  /*
   * Gather some realtime MIDI data for 10 seconds
   */
  while(midi_io.get_currenttime() < 10000){
    event_read = midi_io.read_event(event);
    if(event_read){
      eventlist.push_back(event);
    }
    else usleep(10000);
  } // while


  /*
   * Play back what we've just captured
   */
  event_iterator=eventlist.begin();
  midi_io.reset_timebase();
  while(event_iterator != eventlist.end()){
    if(midi_io.get_currenttime() >= event_iterator->timestamp){
      // It's time to play
      event = *event_iterator;
      midi_io.write_event(&event);
      event_iterator++;
    } // if
    else usleep(10000);
  } // while

  midi_io.finalise();
  return 0;
}

