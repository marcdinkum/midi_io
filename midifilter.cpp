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
*  File name     : midifilter.cpp
*  System name   : MIDI I/O
* 
*  Description   : Only let note on and off pass.
*
*		   This enables you to ignore all the rubbish that sequencers
*		   seem to find necessary to squirt into your synth.
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
bool event_read;
int input_device=0,output_device=0;
bool use_default_devices=false;

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
  midi_io.set_input_filter(PM_FILT_PROGRAM); // block program change

  std::cout << "Starting..." << endl;

  /*
   * Gather some realtime MIDI data
   */
  while(true){
    event_read = midi_io.read_event(event);
    if(event_read){
      midi_io.write_event(&event);
    }
    else usleep(10000);
  } // while

  midi_io.finalise();
  return 0;
}

