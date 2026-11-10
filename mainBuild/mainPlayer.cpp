#include "DSP.h"
#include "controller.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

int blocksize = 379; 
int samplerate = 8000; 
int channels = 1; 
std::vector<int> toner = {};
std::vector<bool> executed = {false, false};
std::mutex mtx;

void player1()
{
    int playerNumber = 0;
    int robotIDudp = 0;
    int robotIDtcp = 1;
    std::vector<int> data = {0, 0, 0, 0}; 
    int lastPressedKey = -1;

    DSP dsp(blocksize, samplerate, channels, {robotIDudp, robotIDtcp});

    Controller input;    
    while(1)
    {
        mtx.lock();
        int pakkenr = 0;
        switch (input.readKey()) {
            case Key::Up:
                std::cout << "Up arrow pressed" << std::endl;
                if (lastPressedKey != 1){
                    lastPressedKey = 1;
                    data = {robotIDudp, 7, 7, 7};
                    executed[playerNumber] = false;
                }
                break;
            case Key::Down:
                std::cout << "Down arrow pressed" << std::endl;
                if (lastPressedKey != 2){
                    lastPressedKey = 2;
                    data = {robotIDtcp, pakkenr/16, pakkenr%16, 15, 11, 15};
                    executed[playerNumber] = false;
                }
                pakkenr++;
                break;
            case Key::Left:
                std::cout << "Left arrow pressed" << std::endl;
                if (lastPressedKey != 3){
                    lastPressedKey = 3;
                    data = {robotIDudp, 15, 15, 15};
                    executed[playerNumber] = false;
                }
                break;
            case Key::Right:
                std::cout << "Right arrow pressed" << std::endl;
                if (lastPressedKey != 4){
                    lastPressedKey = 4;
                    data = {robotIDudp, 0, 0, 0};
                    executed[playerNumber] = false;
                }
                break;
            default:
                std::cout << "Other key pressed" << std::endl;
                break;
            }
        mtx.unlock();
    }

}

void player2()
{
    int playerNumber = 1;
    int robotIDudp = 2;
    int robotIDtcp = 3;
    std::vector<int> data = {0, 0, 0, 0};   
    int lastPressedKey = -1;

    DSP dsp(blocksize, samplerate, channels, {robotIDudp, robotIDtcp});

    Controller input;    
    while(1)
    {
        mtx.lock();
        int pakkenr = 0;
        switch (input.readKey()) {
            case Key::W:
                std::cout << "W key pressed" << std::endl;
                if (lastPressedKey != 1){
                    lastPressedKey = 1;
                    data = {robotIDudp, 7, 7, 7};
                    executed[playerNumber] = false;
                }
                break;
            case Key::S:
                std::cout << "S key pressed" << std::endl;
                if (lastPressedKey != 2){
                    lastPressedKey = 2;
                    data = {robotIDtcp, pakkenr/16, pakkenr%16, 15, 11, 15};
                    executed[playerNumber] = false;
                }
                pakkenr++;
                break;
            case Key::A:
                std::cout << "A key pressed" << std::endl;
                if (lastPressedKey != 3){
                    lastPressedKey = 3;
                    data = {robotIDudp, 15, 15, 15};
                    executed[playerNumber] = false;
                }
                break;
            case Key::D:
                std::cout << "D key pressed" << std::endl;
                if (lastPressedKey != 4){
                    lastPressedKey = 4;
                    data = {robotIDudp, 0, 0, 0};
                    executed[playerNumber] = false;
                }
                break;
            default:
                std::cout << "Other key pressed" << std::endl;
                break;
            }
        mtx.unlock();
    }


}

void queue()
{


}

int main()
{


    return 0;
}