#include "DSP.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <stdexcept>

#include <cstdint>
#include <vector>
#include <stdexcept>

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


DSP::DSP(int blockSize, int samplerate, int channels)
    : stream(nullptr),
      blocksize(blockSize),
      channel(channels)
{   
    err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error(Pa_GetErrorText(err));
    }

    PaDeviceIndex device = Pa_GetDefaultInputDevice();

    if (device == paNoDevice) {
        throw std::runtime_error("Ingen mikrofon fundet!");
    }

    const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
    
    err = Pa_OpenDefaultStream(
        &stream,
        channel,
        0,
        paFloat32,
        samplerate,
        blocksize,
        nullptr,
        nullptr
    );

    if (err != paNoError) {
        Pa_Terminate();
        throw std::runtime_error(Pa_GetErrorText(err));
    }

    err = Pa_StartStream(stream);

    if (err != paNoError) {
        Pa_CloseStream(stream);
        stream = nullptr;
        Pa_Terminate();
        throw std::runtime_error(Pa_GetErrorText(err));
    }

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
    fileFromData(filename, mData); // Skriv mData til fil
}

void DSP::fileFromData(std::string filename, const std::vector<float>& data) {
    static bool firstTime = true;

    std::ofstream file;

    if (firstTime) {
        file.open(filename, std::ios::trunc);
        firstTime = false;
    }
    else {
        file.open(filename, std::ios::app);
    }

    for (float sample : data) {
        file << sample << '\n';
    }
}

void DSP::toFile(std::string filename, std::vector<double> data) {
    //Vi bruger til at gemme mData i en data.txt fil.
    std::ofstream file(filename);
    for(auto i : data){
        file << i << '\n';
    }
}

std::vector<double> DSP::goertzel(int N, double fs, bool useDtmf) {
    return goertzel(mData, N, fs, useDtmf);
}

std::vector<double> DSP::goertzel(std::vector<float> data, int N, double fs, bool useDtmf){
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
        for(double x : data){
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

int DSP::getTone(const std::vector<double>& magnitude, bool useDtmf) {
    //Bruges til at finde hvilken dtmf tone der matcher magnitude spektrummet.
    //Finder højste og næste højeste værdi i magnitude

    double threshold = sum(magnitude)/5; // Laver en threshold værdi som er 1/5 af summen af magnitude. Dette bruges til at filtrere støj væk.
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
    if (magnitude[index1] < threshold || magnitude[index2] < threshold || magnitude[index1] < 0.002 || magnitude[index2] < 0.002) {
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

std::vector<std::vector<double>> DSP::window(int blocksize, int hopSize, double fs, bool useDtmf) {
    std::vector<std::vector<double>> windowMagnitude;

    if (blocksize <= 0 || hopSize <= 0) {
        throw std::invalid_argument("Invalid window parameters");
    }

    for (size_t n = 0; n + blocksize <= mData.size(); n += hopSize) {

        std::vector<float> dataWindow(
            mData.begin() + n,
            mData.begin() + n + blocksize
        );

        windowMagnitude.push_back(goertzel(dataWindow, blocksize, fs, useDtmf));
    }

    return windowMagnitude;
}

double DSP::sum(const std::vector<double> &vec) {
    double total = 0;
    for (double num : vec) {
        total += num;
    }
    return total;
}

void DSP::waitForEnergy() {
    //Funktion bruges til at vente på at der er energi i signalet.
    bool energyDetected = false;
    std::vector<double> magnitude;
    while (!energyDetected) {
        Gathering();
        magnitude = goertzel(20, 8000, true);
        if (sum(magnitude) > 0.5) { // Threshold for energy detection
            energyDetected = true;
        }
    }
}

void DSP::listen(int samplerate, int blocksize) {
    //Funktion bruges til at lytte efter en tone og udskrive den til terminalen.
    //std::cout << "Lytter efter tone..." << std::endl;
    int sluttone = 16;
    int tone = 0;
    std::vector<double> toner = {};
    while(tone != sluttone){
        Gathering(); 
        std::vector<double> magnitude = goertzel(blocksize, samplerate, true);
        tone = getTone(magnitude);
        toner.push_back(tone);
        fileFromData("data.txt");
    }
    toFile("toner.txt", toner);
}


void DSP::initStart(int samplerate, int blocksize, int nIdBits, float timeout) {

    // 40 ms analysevindue og 10 ms hop
    const int WINDOW_SIZE = samplerate * 40 / 1000;
    const int HOP_SIZE = samplerate * 10 / 1000;

    if (samplerate <= 0 || blocksize <= 0 ||
        WINDOW_SIZE <= 0 || HOP_SIZE <= 0 ||
        nIdBits <= 0 || timeout <= 0 ||
        channel != 1 || blocksize != this->blocksize) {
        throw std::invalid_argument("Invalid parameters");
    }

    const std::uint64_t timeoutSamples =
        static_cast<std::uint64_t>(timeout * samplerate);

    enum class State {
        WAIT_ID,
        WAIT_PAUSE,
        LISTEN
    };

    State state = State::WAIT_ID;

    std::vector<float> audioBuffer;
    std::vector<double> toner;

    int numStartBits = 0;
    int endCount = 0;

    std::uint64_t samplePosition = 0;
    std::uint64_t firstIdEnd = 0;

    std::cout << "Venter på startsignal..." << std::endl;

    while (true) {

        // Indsaml nye samples
        Gathering();

        // Tilføj dem til den eksisterende buffer
        audioBuffer.insert(
            audioBuffer.end(),
            mData.begin(),
            mData.end()
        );

        // Analyser alle komplette vinduer
        while (audioBuffer.size() >=
               static_cast<size_t>(WINDOW_SIZE)) {

            std::vector<float> dataWindow(
                audioBuffer.begin(),
                audioBuffer.begin() + WINDOW_SIZE
            );

            std::vector<double> magnitude =
                goertzel(
                    dataWindow,
                    WINDOW_SIZE,
                    samplerate,
                    true
                );

            int tone = getTone(magnitude, true);

            std::uint64_t frameEnd =
                samplePosition + WINDOW_SIZE;

            // STATE 1: Vent på tone 1
            if (state == State::WAIT_ID) {

                if (tone == 1) {
                    numStartBits++;
                }
                else {
                    numStartBits = 0;
                }

                if (numStartBits >= nIdBits) {

                    std::cout << "ID modtaget!" << std::endl;

                    state = State::WAIT_PAUSE;
                    numStartBits = 0;
                    firstIdEnd = frameEnd;
                }
            }

            // STATE 2: Vent på pause
            else if (state == State::WAIT_PAUSE) {

                if (frameEnd - firstIdEnd > timeoutSamples) {

                    std::cout << "Timeout!" << std::endl;

                    state = State::WAIT_ID;
                    numStartBits = 0;
                }
                else {

                    if (tone == -1) {
                        numStartBits++;
                    }
                    else {
                        numStartBits = 0;
                    }

                    if (numStartBits >= nIdBits) {

                        std::cout << "Startsignal modtaget!"
                                  << std::endl;

                        state = State::LISTEN;
                        numStartBits = 0;
                    }
                }
            }

            // STATE 3: Modtag DTMF
            else if (state == State::LISTEN) {
                toner.push_back(tone);

                if (tone == 16) {
                    endCount++;
                }
                else {
                    endCount = 0;
                }

                // Bekræft sluttonen i 3 vinduer
                if (endCount >= 1) {

                    std::cout << "Sluttone modtaget!"
                              << std::endl;

                    toFile("toner.txt", toner);

                    return;
                }
            }

            // Fjern kun HOP_SIZE samples
            audioBuffer.erase(
                audioBuffer.begin(),
                audioBuffer.begin() + HOP_SIZE
            );

            samplePosition += HOP_SIZE;
            fileFromData("data.txt", dataWindow); // Gem det aktuelle vindue til fil
        }
    }
}