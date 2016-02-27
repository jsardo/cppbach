#include "MidiFile.h"
#include <iostream>

using namespace std;

typedef unsigned char uchar;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    MidiFile outputfile;          // create an empty MIDI file with one track
    outputfile.absoluteTicks();  // time information stored as absolute time
                                         // (will be coverted to delta time when written)
    outputfile.addTrack(2);      // Add another two tracks to the MIDI file
    vector<uchar> midievent;      // temporary storage for MIDI events
    midievent.resize(3);          // set the size of the array to 3 bytes
    int tpq = 120;                  // default value in MIDI file is 48
    outputfile.setTicksPerQuarterNote(tpq);

    // data to write to MIDI file: (60 = middle C)
    // C5 C  G G A A G-  F F  E  E  D D C-
    int melody[50]  = {72,72,79,79,81,81,79,77,77,76,76,74,74,72,-1};
    int mrhythm[50] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2,-1};

    // C3 C4 E C F C E C D B3 C4 A3 F G C-
    int bass[50] =    {48,60,64,60,65,60,64,60,62,59,60,57,53,55,48,-1};
    int brhythm[50]= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,-1};


    // store a melody line in track 1 (track 0 left empty for conductor info)
    int i=0;
    int actiontime = 0;        // temporary storage for MIDI event time
    midievent[2] = 64;         // store attack/release velocity for note command
    while (melody[i] >= 0) {
        midievent[0] = 0x90;      // store a note on command (MIDI channel 1)
        midievent[1] = melody[i];
        outputfile.addEvent(1, actiontime, midievent);
        actiontime += tpq * mrhythm[i];
        midievent[0] = 0x80;      // store a note on command (MIDI channel 1)
        outputfile.addEvent(1, actiontime, midievent);
        i++;
    }

    // store a base line in track 2
    i=0;
    actiontime = 0;             // reset time for beginning of file
    midievent[2] = 64;
    while (bass[i] >= 0) {
        midievent[0] = 0x90;
        midievent[1] = bass[i];
        outputfile.addEvent(2, actiontime, midievent);
        actiontime += tpq * brhythm[i];
        midievent[0] = 0x80;
        outputfile.addEvent(2, actiontime, midievent);
        i++;
    }

    outputfile.sortTracks();            // make sure data is in correct order
    outputfile.write("twinkle.mid"); // write Standard MIDI File twinkle.mid
    return 0;
}

