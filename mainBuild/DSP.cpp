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
# define FS             16000

DSP::~DSP() {
    // Stop og luk streamen
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }
    // Afslut PortAudio
    Pa_Terminate();
}

DSP::DSP(int blockSize, int samplerate, int channels, std::vector<int> ID)
    : stream(nullptr), blocksize(blockSize), channel(channels), ID(ID)
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

std::vector<double> DSP::goertzel(int N, double fs) {
    return goertzel(mData, N, fs);
}

std::vector<double> DSP::goertzel(std::vector<float> data, int N, double fs) {
    const std::vector<double> mFrek = getDtmfFrequencies();
    
    std::vector<double> omega_frek;
    std::vector<double> denFrek;

    for(double f : mFrek){
        double omega = 2*M_PI*f/fs;
        omega_frek.push_back(omega);
        denFrek.push_back(-2*cos(omega));
    }

    //Vector for output Y for forskellige frekvenser oprettes.
    std::vector<std::vector<double>> Y;
    
    // Filter for hver frekvens loopes igennem og beregnes. 
    // Dette er difference funktionen for transferfunktionen af hver bin værdi
    for (std::size_t i = 0; i < mFrek.size(); ++i) {
        int n = 0;
        std::vector<double> y = {0,0};
        for(double x : data){
            y.push_back((x-denFrek[i]*y[n+1]-y[n])); //Formel: y(n) = x-a*y(n-1)-y(n-2)
            n++;
        }
        Y.push_back(y);
    }
    // Magnitude beregnes for hver frekvens ved formlen: sqrt(s[N-1]²+s[N-2]² -2*cos(omega_K)*s[N-1]*s[N-2])
    // Hvor omega_K = vinkelfrekvens = 2*pi*k/N
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

    // Laver den til en single side spectrum.
    for(int i =0; i<magnitude.size();i++){
        magnitude[i] = magnitude[i]*2/N;
    }
    
    return magnitude;

}

int DSP::getTone(std::vector<double>& magnitude) {

    constexpr double PEAK_RATIO = 2.0;
    constexpr double NOISE_MARGIN = 3.0; // Skal måske bruges til at indtænke støj

    const std::vector<double> mFrek = getDtmfFrequencies();

    if (magnitude.size() != mFrek.size() || mFrek.size() != 8) {
        return -1;
    }

    // Find largest magnitude in each frequency group
    auto first = std::max_element(
        magnitude.begin(),
        magnitude.begin() + 4
    );

    auto second = std::max_element(
        magnitude.begin() + 4,
        magnitude.end()
    );

    int index1 = std::distance(magnitude.begin(), first);
    int index2 = std::distance(magnitude.begin(), second);

    double lowPeak = magnitude[index1];
    double highPeak = magnitude[index2];

    // Find second-largest magnitude in each group
    double secondLargestLow = 0.0;
    double secondLargestHigh = 0.0;

    for (int i = 0; i < 4; ++i) {
        if (i != index1) {
            secondLargestLow = std::max(
                secondLargestLow, magnitude[i]
            );
        }
    }

    for (int i = 4; i < 8; ++i) {
        if (i != index2) {
            secondLargestHigh = std::max(
                secondLargestHigh, magnitude[i]
            );
        }
    }

    // Check if each frequency clearly dominates its group
    if (lowPeak <= PEAK_RATIO * secondLargestLow ||
        highPeak <= PEAK_RATIO * secondLargestHigh) {
        return -1;
    }

    // Check against background noise
    // noiseRms must be calibrated and defined elsewhere!
    constexpr double MIN_AMPLITUDE = 0.005;
    if (lowPeak < MIN_AMPLITUDE ||
        highPeak < MIN_AMPLITUDE) {
        return -1;
    }

    double frek1 = mFrek[index1];
    double frek2 = mFrek[index2];

    // Find corresponding DTMF tone
    for (const auto& [key, frequencies] : DTMF1) {
        if ((frequencies.first == frek1 && frequencies.second == frek2) ||
            (frequencies.first == frek2 && frequencies.second == frek1)) {
            return key;
        }
    }

    return -1;
}

std::vector<double> DSP::getDtmfFrequencies() const {
    std::vector<double> frequencies;
    frequencies.reserve(DTMF1.size() * 2);

    for (const auto& mapping : DTMF1) {
        frequencies.push_back(mapping.second.first);
        frequencies.push_back(mapping.second.second);
    }

    std::sort(frequencies.begin(), frequencies.end());
    frequencies.erase(
        std::unique(frequencies.begin(), frequencies.end()),
        frequencies.end()
    );
    return frequencies;
}

std::vector<int> DSP::listen(int samplerate, double toneDuration) {
    std::vector<int> toner;

    const int WINDOW_SIZE = 220;
    const int HOP_SIZE = 11;
    int nIdBits = 10; //Hvor mange id skal høre for at aktivere.

    const int UdpBlockAmount = 7;
    const int TcpBlockAmount = 10;

    std::vector<std::vector<float>> data = {};

    // Beregn Hann-vinduet én gang
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

    // State machine
    enum class State {
        WAIT_ID,
        LISTEN
    };

    State state = State::WAIT_ID;
    std::vector<float> audioBuffer;

    int numStartBitsUDP = 0;
    int numStartBitsTCP = 0;
    int numberOfBlocks = 0;
    int numberOfBlocksWanted = 0;
    std::string protocol = "UDP"; // Default protocol

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

        // Bruges til at stoppe modtagelsen af data når der er modtaget 6 blokke.
        if(state == State::LISTEN){
            numberOfBlocks++;
            if(numberOfBlocks >= numberOfBlocksWanted) {
                // Udskriv toner til terminalen
                for (int t : toner) {
                     std::cout << t << " ";
                }
                std::cout << std::endl;
                std::cout << "Data gemt i filen data.txt" << std::endl;
                for (const std::vector<float>& samples : data) {
                    fileFromData("data.txt", samples);
                }
                std::vector<int> processedData = returnNumberStream(toner, protocol, HOP_SIZE, samplerate, toneDuration);
                return processedData;
            }
        }
 
        // Sliding window    
        while (audioBuffer.size() >=
               static_cast<size_t>(WINDOW_SIZE)) {

            // Opret aktuelt analysevindue
            std::vector<float> dataWindow(
                audioBuffer.begin(),
                audioBuffer.begin() + WINDOW_SIZE
            );

            // Anvend Hann-vinduet
            for (int n = 0; n < WINDOW_SIZE; n++) {
                dataWindow[n] *= hann[n];
            }

            // Beregn Goertzel
            std::vector<double> magnitude =
                goertzel(
                    dataWindow,
                    WINDOW_SIZE,
                    samplerate
                );

            // Korriger amplitude efter Hann
            for (double& m : magnitude) {
                m *= correctionFactor;
            }

            // Find DTMF-tone
            int tone = getTone(magnitude);

            // STATE 1: Vent på ID
            if (state == State::WAIT_ID) {

                if (tone == ID[0]) {
                    numStartBitsUDP++;
                    numberOfBlocksWanted = UdpBlockAmount;
                    toner.push_back(tone);

                } else if(tone == ID[1]) {
                    numStartBitsTCP++;
                    numberOfBlocksWanted = TcpBlockAmount;
                    toner.push_back(tone);
                    protocol = "TCP";
                }

                if (numStartBitsUDP >= nIdBits || numStartBitsTCP >= nIdBits) {
                    state = State::LISTEN;
                }
            }

            // STATE 2: Modtag data
            else if (state == State::LISTEN) {
                    toner.push_back(tone);
                }
            

            // Flyt sliding window
            audioBuffer.erase(audioBuffer.begin(), audioBuffer.begin() + HOP_SIZE);
        }
    }
}

std::vector<float> DSP::tone(int key, double duration) {

    auto frequencies = DTMF1.at(key);

    double f_low = frequencies.first;
    double f_high = frequencies.second;

    int samples = static_cast<int>(duration * FS);

    std::vector<float> signal(samples);

    for (int i = 0; i < samples; i++) {

        double t = static_cast<double>(i) / FS;

        signal[i] = 0.25f * (std::sin(2 * M_PI * f_low * t) + std::sin(2 * M_PI * f_high * t));
    }

    return signal;
}

std::vector<float> DSP::make_signal(const std::vector<int>& keys, double tone_dur, double gap) {
    std::vector<float> signal;
    int silence_samples = static_cast<int>(gap * FS);

    for (int key : keys) {
        std::vector<float> samples = tone(key, tone_dur);

        signal.insert(signal.end(), samples.begin(), samples.end());

        signal.insert(signal.end(), silence_samples, 0.0f);
    }

    return signal;
}

void DSP::sendData(const std::vector<int>& keys, double tone_dur, double gap){
    std::vector<float> signal = make_signal(keys, tone_dur, gap);

    // Add 100 ms silence at the beginning
    signal.insert(signal.begin(), FS / 10, 0.0f);

    // Add 100 ms silence at the end
    signal.insert(signal.end(), FS / 10, 0.0f);

    PaError err = Pa_Initialize();

    if (err != paNoError) {
        std::cerr << "PortAudio initialization error: "
                  << Pa_GetErrorText(err) << std::endl;
        return;
    }
}