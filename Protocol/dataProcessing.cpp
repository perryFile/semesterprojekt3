//dataProcessing.cpp
#include "dataProcessing.h"

DataProcessing::DataProcessing() {}

int DataProcessing::getCommand(std::vector<std::int32_t>& data) {
    // Error handling: check if the data vector is empty
    if (data.empty()) {
        return -1; // Return -1 if the data vector is empty
    }
    return data[0]; // Return the first element as the command

    for (const auto& value : data) {
        if (value == ID){
            command = data[value + 1];
            return command;
        }
        
    }
}

DataProcessing::~DataProcessing() {}