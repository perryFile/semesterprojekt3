//physicalLayer.cpp
#include "physicalLayer.h"

PhysicalLayer::PhysicalLayer() {
    
}

std::vector<std::int32_t> PhysicalLayer::getData() {
 return data;
}

std::vector<std::int32_t> PhysicalLayer::receiveData(std::vector<std::int32_t> inputData) {
    data = inputData;
    
    return data;
}
