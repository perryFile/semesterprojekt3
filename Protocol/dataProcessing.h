//DataProcessing.h
#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class DataProcessing {
    public:
    DataProcessing(){};
    int getCommand(int ID,std::vector<std::int32_t>& data);
    ~DataProcessing() = default;
    
    private: 
    int stopbit = 16; 
    

}

#endif // DATAPROCESSING_H