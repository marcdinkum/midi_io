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
*  File name     : midifile.cpp
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

#include <iostream>
#include <fstream>
#include <netinet/in.h>		// ntohs,ntohl,htons,htonl

#include "midifile.h"



#define META_EVENT		0xff
#define TEMPO_META_EVENT	0x51
#define END_OF_TRACK_META_EVENT	0x2f
#define TRACKNAME_META_EVENT	0x03


#define RESOLUTION	100	// usec
#define MAXMSGLENGTH	10	// max length of message


/* Timing considerations

   Suppose we use a value of 120 beats per minute. This means we get
   120 quarter-notes per minute, or two per second.

   ** Tempo meta event **
   Tempo is set in usec/beat. One minute has 60E+6 usecs.
   If we set the tempo, using a tempo meta-event, to 60E+6 / 120
   the playback will be done at a rate of 120 beats per minute.

   The division (MThd) indicates the number of ticks per beat, provided
   we set the MSB of the first byte.
   With division we can set the resolution.
   Suppose we want a resolution of 0.1 milliseconds,
   (100 usecs) we have to set the division to
   ticks_per_second / beats_per_second = 
   (1E+6 / 100) / (120/60)  = 5000

   Now, duration is specified in ticks which have a resolution of 0.1 msec.
*/


/*
 * The following unions are helping in endian-independent communication
 */

  union ns {
    char array[2];
    int16_t num;
  } netshort;

  union nl {
    char array[4];
    int32_t num;
  } netlong;

  union nf {
    char array[4];
    float num;
  } netfloat;



MidiFile::MidiFile()
{
  bpm=120;
  division= (unsigned short)((1000000.0/RESOLUTION)/(bpm/60.0));
  msgbytes = new unsigned char[MAXMSGLENGTH];
  tracklength = 0;
} // MidiFile()


void MidiFile::open(char* filename)
{
  outfile.open(filename);
} // MidiFile()


void MidiFile::open(std::string filename)
{
  outfile.open(filename.c_str());
} // MidiFile()


void MidiFile::set_bpm(unsigned long bpm)
{
  this->bpm=bpm;
  division= (unsigned short)((1000000.0/RESOLUTION)/(bpm/60.0));
  msgbytes = new unsigned char[MAXMSGLENGTH];
} // MidiFile()


MidiFile::~MidiFile()
{
  outfile.close();
} // ~MidiFile()



/* Variable Int notation
   Shift out chunks of 7 bytes with bit 7 set except for last chunk
   The result is written to the MIDI file
*/
int MidiFile::varint(unsigned long num)
{
int byte;
int top;
unsigned long bit=0x80000000;

  for(top=31;top>=0;top--){
    if(num & bit)break;
    bit = bit >> 1;
  }

  for(byte=top/7;byte>0;byte--){
    msgbytes[byte]=(num & 0x7f);
    if(byte < top/7)msgbytes[byte] |= 0x80;
    num = num >> 7;
  }
  msgbytes[0]=num;
  if(top/7 > 0) msgbytes[0] |= 0x80;
  outfile.write((const char*)msgbytes,top/7+1);
  tracklength += top/7 + 1;
  return 0;
} // varint()



int MidiFile::write_header()
{
unsigned long length;
/*
 * Format:
 * 0 - header-chunk and a single track-chunk w all the note and tempo information
 * 1 - header-chunk and one or more track-chunks, all tracks played simultaneously
 *     The first track is known as the 'Tempo Map' and should contain all meta-events
 *     of the types Time Signature, and Set Tempo
 * 2 - header-chunk and one or more independent track-chunks
 *
 * seq24 requires at least format 1.
 */
unsigned short format=1; // MIDI file format 0, 1 or 2
unsigned short tracks=1;

  // file header
  //outfile << "MThd";
  msgbytes[0]='M';
  msgbytes[1]='T';
  msgbytes[2]='h';
  msgbytes[3]='d';
  outfile.write((const char*)msgbytes,4);

  length=6;
  netlong.num=htonl(length);
  outfile.write(netlong.array,4);

  netshort.num=htons(format);
  outfile.write(netshort.array,2);
  netshort.num=htons(tracks);
  outfile.write(netshort.array,2);
  netshort.num=htons(division);
  outfile.write(netshort.array,2);

  return 0;
}

int MidiFile::start_track()
{

  tracklength = 0; // start with clean slate

  //outfile << "MTrk";
  msgbytes[0]='M';
  msgbytes[1]='T';
  msgbytes[2]='r';
  msgbytes[3]='k';
  outfile.write((const char*)msgbytes,4);


  /*
   * placeholder for chunk size (i.e. tracklength), to be written when we
   *  close the track and know the eventual length, as calculated.
   *  tracklength specifies all bytes in the track _following_ the length
   *  variable, so it does not include the 4 bytes for the length specifier.
   */
  netlong.num=htonl(tracklength);
  outfile.write(netlong.array,4);

  // TEMPO meta event: usecs per quarter note
  msgbytes[0]=0x00; // delta
  msgbytes[1]=META_EVENT;
  msgbytes[2]=TEMPO_META_EVENT;
  msgbytes[3]=0x03; // 3 data bytes
  outfile.write((const char*)msgbytes,4);
  tracklength += 4;

  // write the beats per minute (previously mentioned 3 data bytes)
  netlong.num=(long)(60000000.0/bpm);
  msgbytes[0]=netlong.array[2];
  msgbytes[1]=netlong.array[1];
  msgbytes[2]=netlong.array[0];
  outfile.write((const char*)msgbytes,3);
  tracklength += 3;

  return 0;
} // start_track()


int MidiFile::write_trackname(std::string name)
{
  varint(0); // delta as with all events
  // track name meta event
  msgbytes[0]=META_EVENT; // meta
  msgbytes[1]=TRACKNAME_META_EVENT;
  outfile.write((const char*)msgbytes,2);
  tracklength += 2;

  varint(name.length());

  char *stringarray = (char *)name.c_str();
  outfile.write((const char*)stringarray,name.length());
  tracklength += name.length();

  return 0;
} // end_track()


int MidiFile::end_track()
{
  //varint(10000); // delta as with all events
  varint(0); // delta
  // END OF TRACK meta event
  msgbytes[0]=META_EVENT; // meta
  msgbytes[1]=END_OF_TRACK_META_EVENT;
  msgbytes[2]=0; // event length (varint)
  outfile.write((const char*)msgbytes,3);
  tracklength += 3;

  return 0;
} // end_track()


int MidiFile::write_event(unsigned long delay,char *bytes,unsigned long nrofbytes)
{
  varint(delay);
  outfile.write(bytes,nrofbytes);
  tracklength += nrofbytes;

  return 0;
} // write_event()


int MidiFile::close()
{

  // update tracklength in MTrk
  outfile.seekp(18);
  netlong.num=htonl(tracklength);
  outfile.write(netlong.array,4);

  outfile.close();
  return 0;

} // close()


