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
#include <cstdlib>
#include "midi_io.h"


double qnote_duration(double bpm)
{
  return 60000./bpm;
} // qnote_duration()


int main(int argc, char **argv)
{
MIDI_io midi_io;
vector<PmEvent> eventlist;
vector<PmEvent>::iterator event_iterator;
int output_device=0;
bool use_default_devices=false;
double bpm=60;
double bpm_increment=0;
long next_timestamp=0;
unsigned char accent_note=35;
unsigned char default_note=37;
unsigned int beats_per_bar=4;
unsigned int beat=0;

  std::cout << "metronome [bpm] [bpm_increment] [beats-per-bar] [accent-note] [default-note]\n\n\n";

  if(argc > 1){
    bpm=atof(argv[1]);
  }
  if(argc > 2){
    bpm_increment=atof(argv[2]);
  }
  if(argc > 3){
    beats_per_bar=atoi(argv[3]);
  }
  if(argc > 4){
    accent_note=atoi(argv[4]);
  }
  if(argc > 5){
    default_note=atoi(argv[5]);
  }

  midi_io.list_devices();

  if(!use_default_devices){
    std::cout << "Give output device number: ";
    cin >> output_device;
    midi_io.set_output_device(output_device);
  }

  midi_io.initialise();
  midi_io.reset_timebase();

  PmEvent event;
  event.timestamp=0;
  while(bpm < 160)
  {
    if(midi_io.get_currenttime() >= next_timestamp){
      if(!(beat%beats_per_bar)) event.message=Pm_Message(0x99,accent_note,100);
      else event.message=Pm_Message(0x99,default_note,100);
      midi_io.write_event(&event);
      next_timestamp=midi_io.get_currenttime()+qnote_duration(bpm);
      bpm += bpm_increment;
      std::cout << "Beat # " << beat << " BPM: " << bpm << endl;
      ++beat;
    }
    else usleep(10000);
  }

  midi_io.finalise();
  return 0;
}

