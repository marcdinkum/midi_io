/**********************************************************************
*          Copyright (c) 2008, Hogeschool voor de Kunsten Utrecht
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
*  File name     : midi_io.cpp
*  System name   : MIDI I/O
* 
*  Description   : MIDI input and output implementation file
*		   Wrapper class for portmidi
*
*
*  Author        : Marc_G
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#include <iostream>
#include <cstdlib>
#include "midi_io.h"

using namespace std;



MIDI_io::MIDI_io()
{
  active=false;
  input_device=-1;
  output_device=-1;
} // MIDI_io()


MIDI_io::~MIDI_io()
{
} // ~MIDI_io()


long MIDI_io::get_currenttime()
{
  unsigned long secdiff,usecdiff;

  gettimeofday(&tv,&tz);
  secdiff= (tv.tv_sec - tv_zero.tv_sec) * 1000000/ RESOLUTION;
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
  Pm_Write(midi_out,event,1);
} // write_event()


int MIDI_io::list_devices()
{
const PmDeviceInfo *info;

  if(Pm_CountDevices() == 0){
    cout << "No devices found" << endl;
    return 0;
  } // if

  for(int d=0;d<Pm_CountDevices();d++)
  {
    info = Pm_GetDeviceInfo(d);
    cout << "Device " << d << "\t" << info->name;
    if(info->input > 0) cout << " IN";
    if(info->output > 0) cout << " OUT";
    cout << endl;
  } // for

  return Pm_CountDevices();
} // list_devices()


void MIDI_io::set_input_device(int device)
{
  input_device=device;
} // set_input_device()


void MIDI_io::set_output_device(int device)
{
  output_device=device;
} // set_output_device()


/*
 * Set up midi processing thread and open midi streams
 *
 * Don't make any calls to PortMidi from this thread once
 *  the midi thread begins.
 */
void MIDI_io::initialise()
{
const PmDeviceInfo *info;

  tz.tz_minuteswest= -60; // set time zone
  reset_timebase();

  Pm_Initialize();

  /*
   * INPUT device
   */
  if(input_device == -1) input_device=Pm_GetDefaultInputDeviceID();
  info = Pm_GetDeviceInfo(input_device);
  if (info == NULL) {
      cout << "Could not open input device " <<  input_device << endl;
      exit(0);
  }
  cout << "Opening input device " << info->interf << "," <<  info->name << endl;

  Pm_OpenInput(&midi_in, 
	       input_device, 
	       NULL, // driver info
	       0, // use default input size
	       NULL,
	       (void *)NULL); // void * time_info

  // Filter everything but note events
  Pm_SetFilter(midi_in, PM_FILT_REALTIME | PM_FILT_AFTERTOUCH |
    PM_FILT_PROGRAM | PM_FILT_PITCHBEND | PM_FILT_CONTROL);


  /*
   * OUTPUT device
   */
  if(output_device == -1) output_device=Pm_GetDefaultOutputDeviceID();
  info = Pm_GetDeviceInfo(output_device);
  if (info == NULL) {
      cout << "Could not open output device << " << output_device << endl;
      exit(0);
  }
  cout << "Opening output device " << info->interf << "," <<  info->name << endl;

  /* use zero latency because we want output to be immediate */
  Pm_OpenOutput(&midi_out, 
		output_device, 
		NULL, // driver info
		OUT_QUEUE_SIZE,
		NULL, // PmTimeProcPtr time_proc
		NULL, // void * time_info
		0 // Latency
		);


  active = true;
} // initialise()


void MIDI_io::finalise()
{
    active = false;

    /* At this point, midi thread is inactive and we need to shut down
     * the midi input and output
     */
    Pm_Close(midi_in);
    Pm_Close(midi_out);
    Pm_Terminate();    
} // finalise()

