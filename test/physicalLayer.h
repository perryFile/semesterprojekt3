// physicalLayer.h
#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class PhysicalLayer {
    public: 
    PhysicalLayer();
    std::vector<std::int32_t> getData();
    std::vector<std::int32_t> receiveData(std::vector<std::int32_t> inputData);

    ~PhysicalLayer() = default;
    

    private: 
        std::vector<std::int32_t> data; 
};

#endif // PHYSICALLAYER_H