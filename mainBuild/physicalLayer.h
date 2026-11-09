// physicalLayer.h
#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include "DSP.h"

class PhysicalLayer {
    public: 
        PhysicalLayer(int blockSize = 379, int samplerate = 8000, int channels = 1, std::vector<int> ID = {0, 1});
        std::vector<std::int32_t> receiveData();
        void queueData(std::vector<std::int32_t> inputData);
        std::vector<std::int32_t> getData();
        void sendData(std::vector<std::int32_t> inputData);
        std::vector<int> listen(int samplerate, double toneDuration = 0.07);
        ~PhysicalLayer() = default;
    
    private: 
        std::vector<std::int32_t> data;
        std::queue<std::vector<std::int32_t>> queuedData;
        DSP dsp; // Instance of DSP class for audio processing
};

#endif // PHYSICALLAYER_H