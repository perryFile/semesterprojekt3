// physicalLayer.h
#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "DSP.h"
#include "queue.h"

class PhysicalLayer {
    public: 
        PhysicalLayer(Queue& pLTodP, Queue& dPTopL,int blockSize = 379, int samplerate = 8000, int channels = 1, std::vector<int> ID = {0, 1});
        std::vector<std::int32_t> receiveData();
        void sendData(std::vector<std::int32_t> inputData);
        void queueData(std::vector<std::int32_t> inputData);
        std::vector<int> listen(int samplerate, double toneDuration = 0.07);
        void requestStop(); // Request to stop the DSP listening thread
        ~PhysicalLayer() = default;

    private: 
        std::vector<std::int32_t> data;
        DSP dsp; // Instance of DSP class for audio processing
        Queue& pLTodP; // Reference to the queue for data from PhysicalLayer to DataProcessing
        Queue& dPTopL; // Reference to the queue for data from DataProcessing to PhysicalLayer
};

#endif // PHYSICALLAYER_H