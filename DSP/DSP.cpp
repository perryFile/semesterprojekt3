#include "DSP.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

# define M_PI           3.14159265358979323846

DSP::~DSP() {
    // Stop og luk streamen
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }
    // Afslut PortAudio
    Pa_Terminate();
}

DSP::DSP(int blockSize, int samplerate, int channels) : err(Pa_Initialize()), stream(nullptr), blocksize(blockSize), channel(channels) {


    // Initialiser PortAudio

    if (err != paNoError) {
        std::cerr << Pa_GetErrorText(err) << std::endl;
        return;
    }   


    // Åbn mikrofonen
    err = Pa_OpenDefaultStream(
        &stream,
        channels,
        0,
        paFloat32,
        samplerate,
        blockSize,
        nullptr,
        nullptr
    );

    if (err != paNoError) {
        std::cerr << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return;
    }
    // Start optagelsen
    err = Pa_StartStream(stream);

    if (err != paNoError) {
        std::cerr << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }
    // Alloker plads til samples
    mData.resize(blocksize * channel);

}

void DSP::Gathering() {
    //Funktion bruges til at gemme en mængde data i mData vectoren.

    // Indsaml samples
    err = Pa_ReadStream(
        stream,
        mData.data(),
        blocksize
    );

    if (err != paNoError) {
        std::cerr << Pa_GetErrorText(err) << std::endl;
    }

}

std::vector<float> DSP::dataReturn() {
    return mData;
}

void DSP::fileFromData(std::string filename) {
    //Vi bruger til at gemme mData i en data.txt fil.
    std::ofstream file(filename);
    for(float i : mData){
        file << i << '\n';
    }
}

void DSP::toFile(std::string filename, std::vector<double> data) {
    //Vi bruger til at gemme mData i en data.txt fil.
    std::ofstream file(filename);
    for(auto i : data){
        file << i << '\n';
    }
}

std::vector<double> DSP::goertzel(int N, double fs, bool useDtmf){
    std::vector<int> mFrek;
    if(useDtmf){
        mFrek = mFrek1;
    }
    else{
        mFrek = mFrek2;
    }
    //Vector med bin værdier oprettes
    std::vector<int> k_freks = {};
    //Bin værdier beregnes og tilføjes til vektoren.
    for(double f: mFrek){
        k_freks.push_back(round(f/fs*N));
    }

    //Vi beregner denummerator for hver bin værdi.
    std::vector<double> denFrek = {};
    for(int k: k_freks){
        denFrek.push_back(-2*cos(2*M_PI*k/N));
    }

    //Vector for output Y for forskellige frekvenser oprettes.
    std::vector<std::vector<double>> Y;
    
    //Filter for hver frekvens loopes igennem og beregnes. 
    //Dette er difference funktionen for transferfunktionen af hver bin værdi
    int nn = 0;
    for(int i : mFrek){
        int n = 0;
        std::vector<double> y = {0,0};
        for(double x : mData){
            y.push_back((x-denFrek[nn]*y[n+1]-y[n])); //Formel: y(n) = x-a*y(n-1)-y(n-2)
            n++;
        }
        nn ++;
        Y.push_back(y);
    }
    //Magnitude beregnes for hver frekvens ved formlen: sqrt(s[N-1]²+s[N-2]² -2*cos(omega_K)*s[N-1]*s[N-2])
    //Hvor omega_K = vinkelfrekvens = 2*pi*k/N
    std::vector<double> magnitude = {};
    for (int i =0; i<mFrek.size();i++){
        magnitude.push_back(sqrt(pow(Y[i][N+1],2) + pow(Y[i][N],2) - 2*cos(2*M_PI*k_freks[i]/N)*Y[i][N+1]*Y[i][N]));
    }

    //Laver den til en single side spectrum.
    for(int i =0; i<magnitude.size();i++){
        magnitude[i] = magnitude[i]*2/N;
    }
    
    return magnitude;

}

double DSP::sum(const std::vector<double> &vec) {
    double total = 0;
    for (double num : vec) {
        total += num;
    }
    return total;
}

int DSP::getTone(const std::vector<double>& magnitude, bool useDtmf) {
    //Bruges til at finde hvilken dtmf tone der matcher magnitude spektrummet.
    //Finder højste og næste højeste værdi i magnitude

    double threshold = sum(magnitude)/4; // Laver en threshold værdi som er 1/4 af summen af magnitude. Dette bruges til at filtrere støj væk.
    std::vector<std::vector<int>> dtmf;
    std::vector<int> mFrek;
    if(useDtmf){
        dtmf = dtmf1;
        mFrek = mFrek1;
    }
    else{
        dtmf = dtmf2;
        mFrek = mFrek2;
    }

    if (magnitude.size() < 2 || magnitude.size() != mFrek.size()) {
        return -1;
    }
    auto first = std::max_element(magnitude.begin(), magnitude.end());
    int index1 = std::distance(magnitude.begin(), first);
    auto second = std::max_element(magnitude.begin(), first);
    auto second2 = std::max_element(first + 1, magnitude.end());

    if (second == first || (second2 != magnitude.end() && *second2 > *second))
        second = second2;

    int index2 = std::distance(magnitude.begin(), second);

    int frek1 = mFrek[index1];
    int frek2 = mFrek[index2];
    if (magnitude[index1] < threshold || magnitude[index2] < threshold) {
        return -1; // Returner -1 hvis ingen af de to højeste værdier er over threshold
    }

    //Finder hvilken dtmf tone der matcher de to højeste magnitude.
    for (int i = 0; i < dtmf.size(); i++) {
        if ((dtmf[i][0] == frek1 && dtmf[i][1] == frek2) ||
            (dtmf[i][0] == frek2 && dtmf[i][1] == frek1)) {
            return i + 1;
        }
    }

    return -1;
}