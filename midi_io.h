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
*  File name     : midi_io.h
*  System name   : MIDI I/O
* 
*  Description   : MIDI input and output header file
*		   C++ wrapper class for PortMidi
*
*  Author        : Marc Groenewegen
*  E-mail        : marcg@dinkum.nl
*
*
*  Based on PortMidi, a cross-platform C library for MIDI IO
*  https://github.com/rbdannenberg/portmidi
*
**********************************************************************/

#ifndef _MIDI_IO_H_
#define _MIDI_IO_H_

#include <iostream>
#include <sys/time.h>
#include <unistd.h> // usleep
#include <portmidi.h>

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
 * Applying the Singleton Pattern would be useful.
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
  void set_input_filter(unsigned int filters);
  void create_virtual_input_device(std::string);
  void create_virtual_output_device(std::string);
  int initialise();
  long get_currenttime();
  void reset_timebase();
  bool read_event(PmEvent&);
  void write_event(PmEvent*);
  void finalise();
private:
  bool active; // signals that MIDI processing is running
  PmStream *midi_in;
  PmStream *midi_out;
  int input_device; // id of the selected input device
  int output_device; // id of the selected output device

  // offer real time clock function
  struct timeval tv_zero,tv;
  struct timezone tz;
};

#endif // _MIDI_IO_H_
