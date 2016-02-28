#include "twelvetone.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>

int nrand(int n)
{
    return rand() % n;
}

// write a 12 tone "row" to the track tr.
void make_row(Track& tr) 
{
    srand(time(NULL));
    int tones[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int tone_len = 12;
    int current_tone;
    int index;
    int k = 1;

    std::vector<int> row;

    for (int i = 0; i < tone_len; ++i) {
        current_tone = -1;
        k = 1;
        while (current_tone == -1) {
            index = nrand(tone_len);
            if (tones[index] != -1) {
                current_tone = tones[index];
                tones[index] = -1;
            } else if (index + k < tone_len && tones[index + k] != -1) {
                current_tone = tones[index + k];
                tones[index + k] = -1;
            } else if (index - k >= 0 && tones[index - k] != -1) {
                current_tone = tones[index - k];
                tones[index - k] = -1;
            }
            k++;
        }
        row.push_back(current_tone);
    }
    for (auto t : row) {
        tr.push_back(Note(t, nrand(3)+3, nrand(8)+1));
    }
}

