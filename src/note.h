#include <vector>
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
        int note_to_int() const { return 21 + 12*octave + name; }

        int operator-(const Note& n) {
            return std::abs(note_to_int() - n.note_to_int());
        }
};

typedef std::vector<Note> Track;
