#include "MidiFile.h"
#include "generate_melody.h"
#include <iostream>
#include <vector>

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

/*
class Note {
    public:
        int name;
        int octave;
        int value; 

        Note(int n, int o, int v): name(n % 12), octave(o), value(v) { }
        int note_to_int() { return 21 + 12*octave + name; }
};

typedef std::vector<Note> Track;
*/

void write_midi(std::vector<Track> melodies)
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
}

int main()
{
    srand(1234);
    std::vector<Track> song;
    song.push_back(generate_track(20, 5, 1));
    song.push_back(generate_track(10, 2, 2));

    write_midi(song);
    return 0;
}
