#include <vector>
#include <fstream>
#include <string>
#include "../portaudio/include/portaudio.h"

#ifndef  DSP_H
#define DSP_H


class DSP {
    public:
        DSP(int blockSize, int samplerate, int channels);
        ~DSP(); 
        void Gathering();
        std::vector<float> dataReturn();
        void fileFromData(std::string filename);
        void fileFromData(std::string filename, const std::vector<float>& data);
        void toFile(std::string filename, std::vector<double> data);
        std::vector<double> goertzel(int blocksize, double fs, bool useDtmf = true);
        std::vector<double> goertzel(std::vector<float> data, int blocksize, double fs, bool useDtmf = true);
        std::vector<std::vector<double>> window(int blocksize, int hopSize, double fs, bool useDtmf = true);
        int getTone(const std::vector<double> &magnitude, bool useDtmf = true);
        double sum(const std::vector<double> &vec);
        void listen(int samplerate, int blocksize, int nIdBits);
        void waitForEnergy();

    private:
        std::vector<float> mData;
        std::vector<int> mFrek1 = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
        std::vector<int> mFrek2 = {1697, 1770, 1852, 1941, 2209, 2336, 2477, 2633};
        std::vector<std::vector<int>> dtmf1 = {{697,1209}, {697,1336}, {697,1477}, {697, 1633}, {770,1209}, {770,1336}, {770, 1477}, {770,1633}, {852,1209}, {852,1336}, {852,1477}, {852,1633}, {941,1209},{941,1336}, {941,1477}, {941,1633}};
        std::vector<std::vector<int>> dtmf2 = {{1697,2209}, {1697,2336}, {1697,2477}, {1697, 2633}, {1770,2209}, {1770,2336}, {1770, 2477}, {1770,2633}, {1852,2209}, {1852,2336}, {1852,2477}, {1852,2633}, {1941,2209},{1941,2336}, {1941,2477}, {1941,2633}};
        PaError err; 
        int blocksize; 
        int channel;
        PaStream* stream;
};

#endif // DSP_H