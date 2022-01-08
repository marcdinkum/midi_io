/**********************************************************************
*                Copyright (c) 2021, Marc Groenewegen
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
*  File name     : player.cpp
*  System name   : MIDI I/O
* 
*  Description   : Play a few notes for testing MIDI output
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
int output_device=0;

  midi_io.create_virtual_output_device("midiplaytest");

  midi_io.list_devices();

  std::cout << "Give output device number: ";
  cin >> output_device;
  midi_io.set_output_device(output_device);

  midi_io.initialise();

  /*
   * Build event list
   */
  event.message=Pm_Message(0x90,60,100);
  event.timestamp=0;
  eventlist.push_back(event);
  event.message=Pm_Message(0x90,64,100);
  event.timestamp=500;
  eventlist.push_back(event);
  event.message=Pm_Message(0x90,67,100);
  event.timestamp=1000;
  eventlist.push_back(event);

  /*
   * Play the event list
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

