#pragma once

#include <string>
#include <vector>


double sum(const std::vector<double> &vec);
void toFile(const std::string& filename, const std::vector<float>& data);
std::vector<double> power2magnitude(const std::vector<double>& magnitude);
std::vector<int> returnNumberStream(const std::vector<int>& toner, std::string protocol = "UDP", int jumpSize = 51, int sampleRate = 8000, double toneDuration = 0.07);