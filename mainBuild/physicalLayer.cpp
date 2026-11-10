//physicalLayer.cpp
#include "physicalLayer.h"
#include "DSP.h"

PhysicalLayer::PhysicalLayer(int blockSize, int samplerate, int channels, std::vector<int> ID, Queue& pLTodP, Queue& dPTopL) : dsp(blockSize, samplerate, channels, ID, &pLTodP, &dPTopL), pLTodP(pLTodP), dPTopL(dPTopL) {}

std::vector<std::int32_t> PhysicalLayer::receiveData() {
    // Funktion der skal lytte og hente data ind
    return data;
}

std::vector<int> PhysicalLayer::listen(int samplerate, double toneDuration) {
    // Funktion der skal lytte og hente data ind
    return dsp.listen(samplerate, toneDuration);
}

void PhysicalLayer::sendData(std::vector<std::int32_t> inputData) {
    // Funktion der skal sende data ud
    dsp.sendData(inputData);
    
}

void PhysicalLayer::requestStop() {
    dsp.requestStop();
}

