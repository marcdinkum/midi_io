/**********************************************************************
*          Copyright (c) 2013, Hogeschool voor de Kunsten Utrecht
*                      Hilversum, the Netherlands
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
*  File name     : metronome.cpp
*  System name   : MIDI I/O
* 
*  Description   : metronome with slowly increasing speed for drum training
*
*
*  Author        : Marc_G
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#include <iostream>
#include <vector>
#include "midi_io.h"

using namespace std;

double qnote_duration(double bpm)
{
  return 60000./bpm;
} // qnote_duration()


int main(int argc, char **argv)
{
MIDI_io midi_io;
vector<PmEvent> eventlist;
vector<PmEvent>::iterator event_iterator;
int input_device=0,output_device=0;
bool use_default_devices=false;
double metro_bpm=60;
double bpm_increment=0.1;
long next_timestamp=0;

  midi_io.list_devices();


  if(argc>1 && (string(argv[1]) == "-d")) {
    use_default_devices=true;
    cout << "Using default devices\n";
  }
  else cout << "For using default devices specify -d\n";

  if(!use_default_devices){
    cout << "\nGive input device number: ";
    cin >> input_device;
    midi_io.set_input_device(input_device);
    cout << "Give output device number: ";
    cin >> output_device;
    midi_io.set_output_device(output_device);
  }

  midi_io.initialise();
  midi_io.reset_timebase();

  /*
    #define Pm_Message(status, data1, data2) \
              ((((data2) << 16) & 0xFF0000) | \
               (((data1) << 8) & 0xFF00) | \
               ((status) & 0xFF))
  */
  PmEvent event;
  event.message=Pm_Message(0x99,58,100); // note: drum track!
  event.timestamp=0;
  while(metro_bpm < 140)
  {
    if(midi_io.get_currenttime() >= next_timestamp){
      midi_io.write_event(&event);
      next_timestamp=midi_io.get_currenttime()+qnote_duration(metro_bpm);
      metro_bpm += bpm_increment;
      cout << "BPM: " << metro_bpm << endl;
    }
    else usleep(10000);
  }

  midi_io.finalise();
  return 0;
}

