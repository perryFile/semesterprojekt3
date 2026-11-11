#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include "../portaudio/include/portaudio.h"

#ifndef  DSP_H
#define DSP_H


class DSP {
    public:
        DSP(int blockSize, int samplerate, int channels, std::vector<int> ID);
        ~DSP(); 
        void Gathering();
        std::vector<float> dataReturn();
        void fileFromData(std::string filename);
        void fileFromData(std::string filename, const std::vector<float>& data);
        void toFile(std::string filename, std::vector<double> data);
        std::vector<double> goertzel(int blocksize, double fs);
        std::vector<double> goertzel(std::vector<float> data, int blocksize, double fs);
        int getTone(std::vector<double> &magnitude);
        std::vector<int> listen(int samplerate, double toneDuration = 0.07);
        std::vector<float> tone(int key, double duration = 0.03);
        std::vector<float> make_signal(const std::vector<int>& keys, double tone_dur = 0.03, double gap = 0.04);
        void sendData(const std::vector<int>& keys, double tone_dur = 0.03, double gap = 0.04);

    private:
        std::vector<float> mData;
        std::vector<double> getDtmfFrequencies() const;
        PaError err; 
        int blocksize; 
        int channel;
        PaStream* stream;
        std::vector<int> ID = {};
        int FS = 16000;
        const std::map<int, std::pair<double, double>> DTMF1 = {
            {0,  {697, 1209}}, {1,  {697, 1336}},
            {2,  {697, 1477}}, {3,  {697, 1633}},
            {4,  {770, 1209}}, {5,  {770, 1336}},
            {6,  {770, 1477}}, {7,  {770, 1633}},
            {8,  {852, 1209}}, {9, {852, 1336}},
            {10, {852, 1477}}, {11, {852, 1633}},
            {12, {941, 1209}}, {13, {941, 1336}},
            {14, {941, 1477}}, {15, {941, 1633}}
        };
};

#endif // DSP_H