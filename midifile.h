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
*  File name     : midifile.h
*  System name   : MIDI I/O
*
*  Description   : Writing MIDI files
*                  It doesn't add much to the world, I just wanted to be
*                  able to do this and understand the details
*
*
*  Author        : Marc_G
*  E-mail        : marcg@dinkum.nl
*
**********************************************************************/

#ifndef _MIDIFILE_H_
#define _MIDIFILE_H_

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*
 * MidiFile offers functionality for writing a MIDI file
 */

class MidiFile
{
public:
  MidiFile();
  ~MidiFile();
  void open(char *filename);
  void open(std::string filename);
  void set_bpm(unsigned long bpm);
  int write_header();
  int start_track();
  int write_trackname(std::string name);
  int end_track();
  int write_event(unsigned long delay,char *bytes,unsigned long length);
  int close();
private:
  int varint(unsigned long num);

  ofstream outfile;

  unsigned long bpm;

  /*
   * division: ticks per quarter note or per beat
   * This is specified in file header MThd
   */
  unsigned short division;

  unsigned char *msgbytes; // the actual MIDI bytes
  unsigned long msglength; // length of a MIDI message

  // number of bytes in a track, not including the length variable itself
  unsigned long tracklength;
};

#endif // _MIDIFILE_H_
