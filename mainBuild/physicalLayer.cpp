//physicalLayer.cpp
#include "physicalLayer.h"

PhysicalLayer::PhysicalLayer() {}

std::vector<std::int32_t> PhysicalLayer::receiveData() {
    // Funktion der skal lytte og hente data ind
    return data;
}

void PhysicalLayer::queueData(std::vector<std::int32_t> inputData) {
    queuedData.push(inputData);
}

std::vector<std::int32_t> PhysicalLayer::getData() {
    if (!queuedData.empty()) {
        data = queuedData.front();
        queuedData.pop();
    } else {
        data.clear(); 
    }
    return data;
}

void PhysicalLayer::sendData(std::vector<std::int32_t> inputData) {
 //Mangler Lau og Kirsti kode    
}
