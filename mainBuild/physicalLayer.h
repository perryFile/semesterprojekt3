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

class PhysicalLayer {
    public: 
        PhysicalLayer();
        std::vector<std::int32_t> receiveData();
        void queueData(std::vector<std::int32_t> inputData);
        std::vector<std::int32_t> getData();
        void sendData(std::vector<std::int32_t> inputData);
        ~PhysicalLayer() = default;
    

    private: 
        std::vector<std::int32_t> data;
        std::queue<std::vector<std::int32_t>> queuedData;
};

#endif // PHYSICALLAYER_H