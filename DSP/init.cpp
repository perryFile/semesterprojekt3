#include <vector>
#include <fstream>
#include <string>
#include "init.h"

//Dette er hjælpe funktioner

double sum(const std::vector<double> &vec) {
    double total = 0;
    for (double num : vec) {
        total += num;
    }
    return total;
}

void toFile(const std::string& filename, const std::vector<float>& data) {
    std::ofstream file(filename, std::ios::app);

    for (float sample : data) {
        file << sample << '\n';
    }
}

std::vector<double> power2magnitude(const std::vector<double>& magnitude){
    std::vector<double> power2mag;
    for (double m : magnitude) {
        power2mag.push_back(m * m);
    }
    return power2mag;
}