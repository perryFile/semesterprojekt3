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
#include "init.h"

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
    
    std::vector<double> omega_frek;
    std::vector<double> denFrek;

    for(double f : mFrek){
        double omega = 2*M_PI*f/fs;
        omega_frek.push_back(omega);
        denFrek.push_back(-2*cos(omega));
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

    for (int i = 0; i < mFrek.size(); i++) {
        magnitude.push_back(
            sqrt(
                pow(Y[i][N+1], 2) +
                pow(Y[i][N], 2) -
                2*cos(omega_frek[i])*Y[i][N+1]*Y[i][N]
            )
        );
    }

    //Laver den til en single side spectrum.
    for(int i =0; i<magnitude.size();i++){
        magnitude[i] = magnitude[i]*2/N;
    }
    
    return magnitude;

}

int DSP::getTone(std::vector<double>& magnitude, bool useDtmf) {
    //Bruges til at finde hvilken dtmf tone der matcher magnitude spektrummet.
    //Finder højste og næste højeste værdi i magnitude
    //magnitude = power2magnitude(magnitude);
    double threshold = sum(magnitude)/20; // Laver en threshold værdi som er 1/5 af summen af magnitude. Dette bruges til at filtrere støj væk.
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
    // Find største magnitude blandt de første 8 frekvenser
    auto first = std::max_element(magnitude.begin(), magnitude.begin()+4);

    // Find største magnitude blandt de sidste 8 frekvenser
    auto second = std::max_element(magnitude.begin()+4, magnitude.end());

    // Find indeks
    int index1 = std::distance(magnitude.begin(), first);
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

void DSP::listen(int samplerate, int blocksize, int nIdBits) {

    // Indstillinger for sliding window
    const int WINDOW_SIZE = samplerate * 40 / 1000;
    const int HOP_SIZE = samplerate * 10 / 1000;
    const int END_WINDOWS = 3;

    std::vector<std::vector<float>> data = {};

    // Kontroller parametre
    if (samplerate <= 0 || blocksize <= 0 ||
        WINDOW_SIZE < 2 || HOP_SIZE <= 0 ||
        nIdBits <= 0 ||
        channel != 1 || blocksize != this->blocksize) {
        throw std::invalid_argument("Invalid parameters");
    }

    // ----------------------------------------
    // Beregn Hann-vinduet én gang
    // ----------------------------------------
    std::vector<double> hann(WINDOW_SIZE);

    double windowSum = 0;

    for (int n = 0; n < WINDOW_SIZE; n++) {

        hann[n] = 0.5 * (
            1 - cos(2 * M_PI * n / (WINDOW_SIZE - 1))
        );

        windowSum += hann[n];
    }

    // Korrektionsfaktor for Hann-vinduets dæmpning
    const double correctionFactor = WINDOW_SIZE / windowSum;

    // ----------------------------------------
    // State machine
    // ----------------------------------------
    enum class State {
        WAIT_ID,
        LISTEN
    };

    State state = State::WAIT_ID;

    std::vector<float> audioBuffer;
    std::vector<double> toner;

    int numStartBits = 0;
    int endCount = 0;

    bool ignoreID = true;

    std::cout << "Venter på ID..." << std::endl;

    while (true) {

        // Indsaml nye samples
        Gathering();
        data.push_back(mData);

        // Gem originale samples, hvis ønsket
        // fileFromData("data.txt", mData);

        // Tilføj nye samples til buffer
        audioBuffer.insert(
            audioBuffer.end(),
            mData.begin(),
            mData.end()
        );

        // ----------------------------------------
        // Sliding window
        // ----------------------------------------
        while (audioBuffer.size() >=
               static_cast<size_t>(WINDOW_SIZE)) {

            // Opret aktuelt analysevindue
            std::vector<float> dataWindow(
                audioBuffer.begin(),
                audioBuffer.begin() + WINDOW_SIZE
            );

            // ----------------------------------------
            // Anvend Hann-vinduet
            // ----------------------------------------
            for (int n = 0; n < WINDOW_SIZE; n++) {
                dataWindow[n] *= hann[n];
            }

            // ----------------------------------------
            // Beregn Goertzel
            // ----------------------------------------
            std::vector<double> magnitude =
                goertzel(
                    dataWindow,
                    WINDOW_SIZE,
                    samplerate,
                    true
                );

            // Korriger amplitude efter Hann
            for (double& m : magnitude) {
                m *= correctionFactor;
            }

            // Find DTMF-tone
            int tone = getTone(magnitude, true);

            // ----------------------------------------
            // STATE 1: Vent på ID
            // ----------------------------------------
            if (state == State::WAIT_ID) {

                if (tone == 1) {
                    numStartBits++;
                }
                else {
                    numStartBits = 0;
                }

                if (numStartBits >= nIdBits) {

                    std::cout
                        << "ID modtaget! Starter datamodtagelse."
                        << std::endl;

                    state = State::LISTEN;
                    numStartBits = 0;
                }
            }

            // ----------------------------------------
            // STATE 2: Modtag data
            // ----------------------------------------
            else if (state == State::LISTEN) {

                // Ignorer resterende vinduer med ID-tonen
                if (ignoreID && tone == 1) {
                    // Gør ingenting
                }
                else {

                    ignoreID = false;

                    toner.push_back(tone);

                    // Undersøg om sluttonen er modtaget
                    if (tone == 9) {
                        endCount++;
                    }
                    else {
                        endCount = 0;
                    }

                    // Sluttone bekræftet
                    if (endCount >= END_WINDOWS) {

                        std::cout
                            << "Sluttone modtaget!"
                            << std::endl;

                        // Gem toner i fil
                        // toFile("toner.txt", toner);

                        // Udskriv toner til terminalen
                        for (double t : toner) {
                            std::cout << t << " ";
                        }

                        std::cout << std::endl;
                        std::cout << "Data gemt i filen data.txt" << std::endl;
                        for (const std::vector<float>& samples : data) {
                            fileFromData("data.txt", samples);
                        }

                        return;
                    }
                }
            }

            // ----------------------------------------
            // Flyt sliding window
            // ----------------------------------------
            audioBuffer.erase(
                audioBuffer.begin(),
                audioBuffer.begin() + HOP_SIZE
            );
        }
    }
}