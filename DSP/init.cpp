#include <vector>
#include <fstream>
#include <string>
#include "init.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <cmath>

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

std::vector<int> returnNumberStream(
    const std::vector<int>& toner,
    std::string protocol,
    int jumpSize,
    int sampleRate)
{
    const double toneDuration = 0.05;

    const int streamSize = std::round(toneDuration * sampleRate / jumpSize);

    std::vector<int> tonerProcessed = {};

    if (protocol == "UDP") {

        // Gennemgå vektoren i blokke af streamSize (11)
        for (int i = 0; i + streamSize <= toner.size(); i += streamSize) {

            std::vector<int> segment(
                toner.begin() + i,
                toner.begin() + i + streamSize
            );

            std::unordered_map<int, int> frequency;

            int mostFrequent = -1;
            int maxCount = 0;

            // Find hyppigste tal i segmentet
            for (int k : segment) {
                
                if(k == -1){
                    continue; // Spring over -1 værdier
                }
                else {
                    frequency[k]++;
                }

                if (frequency[k] > maxCount) {
                    maxCount = frequency[k];
                    mostFrequent = k;
                }
            }

            tonerProcessed.push_back(mostFrequent);
            if(tonerProcessed.size() == 4){
                break; // Stop efter at have fundet 4 toner
            }
        }
    }

    else if (protocol == "TCP") {
        for (int i = 0; i + streamSize <= toner.size(); i += streamSize) {

            std::vector<int> segment(
                toner.begin() + i,
                toner.begin() + i + streamSize
            );

            std::unordered_map<int, int> frequency;

            int mostFrequent = -1;
            int maxCount = 0;

            for (int k : segment) {
                if (k == -1) {
                    continue;
                }

                frequency[k]++;
                if (frequency[k] > maxCount) {
                    maxCount = frequency[k];
                    mostFrequent = k;
                }
            }

            tonerProcessed.push_back(mostFrequent);
            if (tonerProcessed.size() == 6) {
                break;
            }
        }
    }

    else {
        std::cout << "Please give either UDP or TCP" << std::endl;
        return {};
    }

    return tonerProcessed;
}