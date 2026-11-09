#include "DSP.h"
#include <iostream>
#include <conio.h>
#include <vector>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77



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
    
    while(1)
    {
        c = 0;

        switch((c=getch())) {
            case KEY_UP:
                cout << endl << "Up" << endl;
                data = {robotIDudp, 7, 7, 7};
                break;
            case KEY_DOWN:
                cout << endl << "Down" << endl;   // key down
                data = {robotIDudp, 0, 0, 0};
                break;
            case KEY_LEFT:
                cout << endl << "Left" << endl;  // key left
                data = {robotIDudp, 15, 15, 15};
                break;
            case KEY_RIGHT:
                cout << endl << "Right" << endl;  // key right
                data = {robotIDudp, 0, 0, 0};
                break;
            default:
                cout << endl << "null" << endl;  // not arrow
                break;
            }

    }
    dsp.sendData(data); 

    return 0;
}