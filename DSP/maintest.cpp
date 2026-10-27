#include "DSP.h"
#include <iostream>
#include <vector>
#include "../portaudio/include/portaudio.h"



// int main(){
//     DSP dsp(220, 8000, 1);
//     int i = 0;
//     std::vector<double> toner = {};
//     for(int j = 0; j < 200; j++) {
//         dsp.Gathering(); //1 channel = mono lyd
//         std::vector<double> magnitude = dsp.goertzel(220, 8000, true);
//         i = dsp.getTone(magnitude);
//         toner.push_back(i);
//         dsp.fileFromData("data.txt");
//     }

//     dsp.toFile("toner.txt", toner);
// }



// int main(){
//     DSP dsp(800, 8000, 1);
//     int i = 0;
//     std::vector<double> toner = {};
//     for(int j = 0; j < 100; j++) {
//         dsp.Gathering(); //1 channel = mono lyd
//         std::vector<std::vector<double>> windowMagnitude = dsp.window(700, 20, 8000, true);
//         for(const auto& magnitude : windowMagnitude) {
//             i = dsp.getTone(magnitude);
//             toner.push_back(i);
//         }
//         dsp.fileFromData("data.txt");
//     }

//     dsp.toFile("toner.txt", toner);
// }

int main(){
    DSP dsp(800, 8000, 1);
    dsp.initStart(8000, 800, 6, 5.0);
}