#include "MidiFile.h"
#include "RtMidi.h"
#include <iostream>
#include <vector>
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

/*
 * convention for note names:
 *   0 <-> A
 *   1 <-> A#
 *   2 <-> B
 *   3 <-> C
 *   4 <-> C# 
 *   5 <-> D
 *   6 <-> D#
 *   7 <-> E
 *   8 <-> F
 *   9 <-> F# 
 *  10 <-> G
 *  11 <-> G#
 */

class Note {
public:
    int name;
    int octave;
    int value; 

    Note(int n, int o, int v): name(n % 12), octave(o), value(v) { }
    int note_to_int() { return 21 + 12*octave + name; }
};

typedef std::vector<Note> Track;

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
    outputfile.write("test.mid");
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
            cout << hex << (int)j << " ";
        }
        cout << endl;

        midiout->sendMessage( &message );
        message.clear();

        if(i < midinotes.size() - 1)
        {
            // cout << midinotes[i+1].time - midinote.time << endl;
            SLEEP((midinotes[i+1].time - midinote.time) * 4);
        }
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


int main()
{
    std::vector<Track> song;
    vector<Note> melody1, melody2, melody3, melody4;

    // A minor arpeggio
    melody1.push_back(Note(3, 5, 1));
    melody1.push_back(Note(0, 5, 1));
    melody1.push_back(Note(3, 5, 1));
    melody1.push_back(Note(7, 5, 1));
    melody1.push_back(Note(3, 5, 1));
    melody1.push_back(Note(7, 5, 1));
    melody1.push_back(Note(0, 6, 1));
    song.push_back(melody1);

    melody2.push_back(Note(0, 4, 1));
    melody2.push_back(Note(7, 3, 1));
    melody2.push_back(Note(0, 4, 1));
    melody2.push_back(Note(3, 4, 1));
    melody2.push_back(Note(0, 4, 1));
    melody2.push_back(Note(3, 4, 1));
    melody2.push_back(Note(7, 4, 1));
    song.push_back(melody2);

    MidiFile midifile = write_midi(song);

    RtMidiOut *midiout = new RtMidiOut();

    midiout->openVirtualPort( "C. P. P. Bach" );

    SLEEP(100);

    play_midi(midiout, song);
    
    return 0;
}
