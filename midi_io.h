/**********************************************************************
*          Copyright (c) 2014, Hogeschool voor de Kunsten Utrecht
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
*  File name     : midi_io.h
*  System name   : MIDI I/O
* 
*  Description   : MIDI input and output header file
*		   Wrapper class for portmidi
*
*
*  Author        : Marc Groenewegen
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#ifndef _MIDI_IO_H_
#define _MIDI_IO_H_

#include <iostream>
#include <sys/time.h>
#include <unistd.h> // usleep
#include "portmidi.h"

using namespace std;

#define OUT_QUEUE_SIZE 1024
#define RESOLUTION      1000     // usec

#define ERROR_OPEN_INPUT	-1
#define ERROR_OPEN_OUTPUT	-2

/*
 * Due to the portmidi callbacks we need to declare a couple of
 *  member functions and the member variables they access **static**
 *
 * Due to this, the programmer must take care that no more than
 *  ONE INSTANCE of this class is created !
 *
 *  NB: static members must be declared outside the class.
 *
 */

class MIDI_io
{
public:
  MIDI_io();
  ~MIDI_io();
  int list_devices();
  void set_input_device(int device);
  void set_output_device(int device);
  int initialise();
  long get_currenttime();
  void reset_timebase();
  void set_input_filter(unsigned int filters);
  bool read_event(PmEvent&);
  void write_event(PmEvent*);
  void finalise();
private:
  bool active; // signals that MIDI processing is running
  PmStream *midi_in;
  PmStream *midi_out;
  // I don't understand PM's time stuff so I'll make my own
  struct timeval tv_zero,tv;
  struct timezone tz;
  int input_device,output_device;
};

#endif // _MIDI_IO_H_
