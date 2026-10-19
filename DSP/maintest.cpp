#include "DSP.h"
#include <iostream>
#include <vector>
#include "../portaudio/include/portaudio.h"


int main(){
    DSP dsp;

    dsp.Gathering(1024,8000,1); //1 channel = mono lyd
    std::vector<float> data = dsp.dataReturn();
    std::vector<double> magnitude = dsp.goertzel(1024,8000, false);
    int i = dsp.getTone(magnitude, false);
    std::cout << i << std::endl;
    dsp.fileFromData();
    
    
}