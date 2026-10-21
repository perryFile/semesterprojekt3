#include "DSP.h"
#include <iostream>
#include <vector>
#include "../portaudio/include/portaudio.h"


int main(){
    DSP dsp(1024, 8000, 1);
    int i = 0;
    std::vector<double> toner = {};
    for(int j = 0; j < 1; j++) {
        dsp.Gathering(); //1 channel = mono lyd
        std::vector<double> magnitude = dsp.goertzel(1024,8000);
        i = dsp.getTone(magnitude);
        toner.push_back(i);
    }
    dsp.toFile("toner.txt", toner);
}