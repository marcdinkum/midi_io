/**********************************************************************
*          Copyright (c) 2022, Hogeschool voor de Kunsten Utrecht
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
*  File name     : midi_io.cpp
*  System name   : MIDI I/O
* 
*  Description   : MIDI input and output implementation file
*		   C++ wrapper class for PortMidi
*
*  Author        : Marc Groenewegen
*  E-mail        : marcg@dinkum.nl
*
*
*  Based on PortMidi, a cross-platform C library for MIDI IO
*  https://github.com/PortMidi/portmidi
*
**********************************************************************/

#include <iostream>
#include <cstdlib>
#include "midi_io.h"


MIDI_io::MIDI_io()
{
  active=false;
  input_device=-1;
  output_device=-1;
} // MIDI_io()


MIDI_io::~MIDI_io()
{
} // ~MIDI_io()


int MIDI_io::list_devices()
{
const PmDeviceInfo *info;

  /*
   * Show devices for selection.
   * Device numbers range from 0 to Pm_CountDevices() - 1
   */

  for(int d=0;d<Pm_CountDevices();d++)
  {
    info = Pm_GetDeviceInfo(d);
    if(info->input > 0) std::cout << "IN from: ";
    if(info->output > 0) std::cout << "OUT to:  ";
    std::cout << "Device " << d << "\t" << info->name;
    if(info->is_virtual > 0) std::cout << " (virtual)";
    std::cout << std::endl;
  } // for

  return Pm_CountDevices();
} // list_devices()


void MIDI_io::set_input_device(int device)
{
  if(device < 0 || device >= Pm_CountDevices()){
    std::cout << "Input device number out of range" << std::endl;
  }
  else input_device=device;
} // set_input_device()


void MIDI_io::set_output_device(int device)
{
  if(device < 0 || device >= Pm_CountDevices()){
    std::cout << "Output device number out of range" << std::endl;
  }
  else output_device=device;
} // set_output_device()


/*
 * Virtual devices can be created independent of exernal devices
 */
void MIDI_io::create_virtual_input_device(std::string name)
{
  Pm_CreateVirtualInput(name.c_str(),
		NULL, // interf
		NULL); // deviceInfo
} // create_virtual_input_device()


void MIDI_io::create_virtual_output_device(std::string name)
{
  Pm_CreateVirtualOutput(name.c_str(),
	      NULL, // interf
	      NULL); // deviceInfo
} // create_virtual_output_device()



/*
 * Input filtering: block certain incoming messages
 *
 * This can only be performed on a valid input port. The function checks this.
 *
 * Useful switches (see the portmidi documentation for more):
 *  PM_FILT_REALTIME
 *  PM_FILT_AFTERTOUCH
 *  PM_FILT_PROGRAM 
 *  PM_FILT_PITCHBEND 
 *  PM_FILT_CONTROL
 *  PM_FILT_RESET
 *
 * These switches can be or'ed 
 */
void MIDI_io::set_input_filter(unsigned int filters)
{
  const PmDeviceInfo *info;
  info = Pm_GetDeviceInfo(input_device);
  if(info->input == 0) return;

  Pm_SetFilter(midi_in,filters);
} // set_input_filter()



/*
 * Set up midi processing thread and open midi streams
 *
 * Don't make any calls to PortMidi from this thread once
 *  the midi thread begins.
 */
int MIDI_io::initialise()
{
const PmDeviceInfo *info;

  tz.tz_minuteswest= -60; // set time zone
  reset_timebase();

  Pm_Initialize();

  /*
   * INPUT device
   */
  if(input_device >= 0){
    info = Pm_GetDeviceInfo(input_device);
    if (info == NULL) {
      return ERROR_OPEN_INPUT;
    }
    std::cout << "Opening input device " << info->interf << "," << info->name << std::endl;

    Pm_OpenInput(&midi_in, // stream
		 input_device, 
		 NULL, // driver info
		 0, // use default input size
		 NULL, // time_proc
		 (void *)NULL); // void * time_info
  } // if input device was selected

  /*
   * OUTPUT device
   */
  if(output_device >= 0){
    info = Pm_GetDeviceInfo(output_device);
    if (info == NULL) {
      return ERROR_OPEN_OUTPUT;
    }
    std::cout << "Opening output device " << info->interf << "," << info->name << std::endl;

    /* use zero latency because we want output to be immediate */
    Pm_OpenOutput(&midi_out,  // stream
		  output_device, 
		  NULL, // driver info
		  OUT_QUEUE_SIZE,
		  NULL, // PmTimeProcPtr time_proc
		  NULL, // void * time_info
		  0); // Latency
  } // if input device was selected

  active = true;

  return 0;
} // initialise()


long MIDI_io::get_currenttime()
{
  unsigned long secdiff,usecdiff;

  gettimeofday(&tv,&tz);
  secdiff= (tv.tv_sec - tv_zero.tv_sec) * 1000000 / RESOLUTION;
  usecdiff= (tv.tv_usec - tv_zero.tv_usec) / RESOLUTION;
  return secdiff+usecdiff;
} // get_currenttime()


void MIDI_io::reset_timebase()
{
  gettimeofday(&tv_zero,&tz); // reset tv_zero
} // reset_timebase()


bool MIDI_io::read_event(PmEvent& event)
{
PmError result;
PmEvent localevent;

  if(input_device < 0) return false;

  while(!active) usleep(10000); // wait for init to complete

  result = Pm_Poll(midi_in);
  if(result > 0) {
    Pm_Read(midi_in,&localevent,1);
    event=localevent;
    event.timestamp=get_currenttime();
    return true; // read an event
  }
  return false; // nothing read
} // read_event()


void MIDI_io::write_event(PmEvent* event)
{
  while(!active) usleep(10000); // wait for init to complete
  if(output_device > 0) Pm_Write(midi_out,event,1);
} // write_event()



void MIDI_io::finalise()
{
    active = false;

    /* At this point, midi thread is inactive and we need to shut down
     * the midi input and output
     */
    if(input_device > 0) Pm_Close(midi_in);
    if(output_device > 0) Pm_Close(midi_out);
    Pm_Terminate();    
} // finalise()

