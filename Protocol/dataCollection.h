// DataCollection.h
#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class DataCollection {
    public: 
    DataCollection(){};
    std::vector<std::int32_t> getData(std::vector<std::int32_t>& data);
    ~DataCollection() = default;

    private: 

}

#endif // DATACOLLECTION_H