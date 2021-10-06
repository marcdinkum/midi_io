
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
bool use_default_devices=false;

  midi_io.list_devices();

  if(argc>1 && (string(argv[1]) == "-d")) {
    use_default_devices=true;
    std::cout << "Using default devices\n";
  }
  else std::cout << "For using default devices specify -d\n";

  if(!use_default_devices){
    std::cout << "Give output device number: ";
    cin >> output_device;
    midi_io.set_output_device(output_device);
  }

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

