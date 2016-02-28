#include "MidiFile.h"
#include "RtMidi.h"
#include "note.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

typedef unsigned char uchar;

MidiFile write_midi(std::vector<Track> melodies)
{
    MidiFile outputfile;
    outputfile.absoluteTicks();

    int melody_count = melodies.size();
    outputfile.addTrack(melody_count);
    std::vector<uchar> midievent;
    midievent.resize(3);
    int tpq = 120; // ticks per quarter note
    outputfile.setTicksPerQuarterNote(tpq);
    
    for (auto melody : melodies) {
        int actiontime = 0;
        midievent[2] = 64;
        for (auto note : melody) {
            midievent[0] = 0x90;
            midievent[1] = note.note_to_int();
            outputfile.addEvent(1, actiontime, midievent);
            actiontime += tpq * note.value;
            midievent[0] = 0x80;
            outputfile.addEvent(1, actiontime, midievent);
        }
    }
    outputfile.sortTracks();
    outputfile.write("compositions/test.mid");
    return outputfile;
}

class MidiNote
{
public:
    int channel;
    int pitch;
    int velocity;
    int on; // 1 on 0 off;
    int time;

    friend bool operator<(MidiNote a, MidiNote b)
    {
        return a.time < b.time;
    }
};

void play_midi(RtMidiOut* midiout, std::vector<Track> tracks)
{
    vector<MidiNote> midinotes;

    for (int i = 0; i < tracks.size(); i++)
    {
        vector<Note> melody = tracks[i];
        int actiontime = 0;
        for (auto note : melody) 
        {
            MidiNote midinote;
            midinote.channel = i;
            midinote.pitch = note.note_to_int();
            midinote.velocity = 64;
            midinote.on = 1;
            midinote.time = actiontime;
            midinotes.push_back(midinote);

            actiontime += 120 * note.value;
            midinote.on = 0;
            midinote.time = actiontime - 1;
            midinotes.push_back(midinote);
        }
    }

    sort(midinotes.begin(), midinotes.end());

    std::vector<unsigned char> message;

    // set channel 0 to grand piano
    message.push_back( 0xC0 );
    message.push_back( 0x01 );
    midiout->sendMessage( &message );
    message.clear();

    // set channel 1 to grand piano
    message.push_back( 0xC1 );
    message.push_back( 0x01 );
    midiout->sendMessage( &message );
    message.clear();

    // set volumn of channel 0 to 0x64
    message.push_back( 0xb0 );
    message.push_back( 0x07 );
    message.push_back( 0x64 );
    midiout->sendMessage( &message );
    message.clear();

    // set volumn of channel 1 to 0x64
    message.push_back( 0xb1 );
    message.push_back( 0x07 );
    message.push_back( 0x64 );
    midiout->sendMessage( &message );
    message.clear();

    for(int i = 0; i < midinotes.size(); i++)
    {
        // Note On: 144, 64, 90
        MidiNote midinote = midinotes[i];

        message.push_back(1 << 7 | midinote.channel);
        if(midinote.on)
        {
            message[0] |= 1 << 4;
        }
        message.push_back(midinote.pitch);
        message.push_back(midinote.velocity);

        for(auto j: message) 
        {
            cout << (int)j << " ";
        }
        cout << midinote.time;
        cout << endl;

        midiout->sendMessage( &message );
        message.clear();

        if(i < midinotes.size() - 1)
        {
            //cout << midinotes[i+1].time << " " << midinote.time << endl;
            SLEEP((midinotes[i+1].time - midinote.time + 1));
        }
        SLEEP(5);
    }
	SLEEP(100);

    // Sysex: 240, 67, 4, 3, 2, 247
    message.push_back( 240 );
    message.push_back( 67 );
    message.push_back( 4 );
    message.push_back( 3 );
    message.push_back( 2 );
    message.push_back( 247 );
    midiout->sendMessage( &message );
}

int zrand()
{
    static int x = 0;
    x += 23 ;
    x *= 4;
    x = x%2394;
    return x+4;
}

int main()
{
    std::vector<Track> song;
    vector<Note> melody1, melody2, melody3, melody4;

    int scale1[] = {5,7,9,0,2};
    int scale1size = 5;
    int shift1 = 0;

    int scale2[] = {3,5,7,8,10,0,2};
    int scale2size = 7;
    int shift2 = 2;

    for(int i = 0; i!= 10; i++) {
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%1+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%1+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%1+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%1+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%2+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%2+4, rand()%4 + 1));
        melody1.push_back(Note(scale1[zrand() % scale1size]+shift1, rand()%3+4, rand()%4 + 1));

        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+1, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+1, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+2, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+2, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+2, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+2, rand()%4 + 1));
        melody2.push_back(Note(scale2[zrand() % scale2size]+shift2, rand()%2+2, rand()%4 + 1));
    }

    song.push_back(melody1);
    song.push_back(melody2);

    MidiFile midifile = write_midi(song);

    RtMidiOut *midiout = new RtMidiOut();

    midiout->openVirtualPort( "C. P. P. Bach" );

    SLEEP(100);

    play_midi(midiout, song);

    return 0;
}
