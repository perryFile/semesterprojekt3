// DataProcessing.h
#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <cstdint>
#include <algorithm>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "physicalLayer.h"

class DataProcessing {
public:
    DataProcessing();
    explicit DataProcessing(const std::vector<std::int32_t>& inputData);
    
    bool checkId(int robotID, const std::vector<std::int32_t>& data) const;
    float getAngle(int robotID) const;
    float getXVelocity() const;
    float getZVelocity(int robotID) const;
    float convertAngleData(const std::vector<std::int32_t>& rawAngle) const;
    ~DataProcessing() = default;

private:
    std::vector<std::int32_t> data;
    std::vector<std::int32_t> command;
    std::vector<std::int32_t> rawAngle;
    float angle = 0.00f;
    float xVelocity = 0.22f;
    static constexpr float maxAngle     = 360.0f;  
    static constexpr float maxZVelocity = 0.22f; 
};

#endif // DATAPROCESSING_H