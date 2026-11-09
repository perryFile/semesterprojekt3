#include "DSP.h"
#include "controller.h"
#include <iostream>
#include <vector>

int main()
{
    int robotIDudp = 0;
    int robotIDtcp = 1;
    std::vector<int> data = {0, 0, 0, 0}; 
    
    int blocksize = 379; 
    int samplerate = 8000; 
    int channels = 1; 
    std::vector<int> toner = {};
    double toneDuration = 0.07; // Duration of each tone in seconds

    DSP dsp(blocksize, samplerate, channels, {robotIDudp, robotIDtcp});
    
    int c = 0;
    Controller input;
    
    while(1)
    {
        c = 0;

        switch (input.readKey()) {
            case Key::Up:
                // UP
                data = {robotIDudp, 7, 7, 7};
                break;
            case Key::Down:
                // DOWN
                data = {robotIDudp, 0, 0, 0};
                break;
            case Key::Left:
                // LEFT
                data = {robotIDudp, 15, 15, 15};
                break;
            case Key::Right:
                // RIGHT
                data = {robotIDudp, 0, 0, 0};
                break;
            default:
                // not arrow
                break;
            }

    }
    dsp.sendData(data); 

    return 0;
}