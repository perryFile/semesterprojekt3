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

    DSP dsp(blocksize, samplerate, channels, {robotIDudp, robotIDtcp});
    
    int c = 0;
    Controller input;
    
    while(1)
    {
        c = 0;

        switch (input.readKey()) {
            case Key::Up:
                std::cout << "Up arrow pressed" << std::endl;
                data = {robotIDudp, 7, 7, 7};
                dsp.sendData(data); 
                break;
            case Key::Down:
                std::cout << "Down arrow pressed" << std::endl;
                data = {robotIDudp, 0, 0, 0};
                dsp.sendData(data); 
                break;
            case Key::Left:
                std::cout << "Left arrow pressed" << std::endl;
                data = {robotIDudp, 15, 15, 15};
                dsp.sendData(data); 
                break;
            case Key::Right:
                std::cout << "Right arrow pressed" << std::endl;
                data = {robotIDudp, 0, 0, 0};
                dsp.sendData(data); 
                break;
            default:
                std::cout << "Other key pressed" << std::endl;
                break;
            }

    }
    

    return 0;
}