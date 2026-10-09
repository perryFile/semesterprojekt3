
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <portaudio.h>

const int FS = 16000;
const double PI = 3.14159265358979323846;

const std::map<char, std::pair<double, double>> DTMF1 = {
    {'1', {697, 1209}}, {'2', {697, 1336}},
    {'3', {697, 1477}}, {'A', {697, 1633}},
    {'4', {770, 1209}}, {'5', {770, 1336}},
    {'6', {770, 1477}}, {'B', {770, 1633}},
    {'7', {852, 1209}}, {'8', {852, 1336}},
    {'9', {852, 1477}}, {'C', {852, 1633}},
    {'*', {941, 1209}}, {'0', {941, 1336}},
    {'#', {941, 1477}}, {'D', {941, 1633}}
};

// Genererer en DTMF-tone
std::vector<float> tone(char key, double duration = 0.1) {
    auto frequencies = DTMF1.at(key);

    double f_low = frequencies.first;
    double f_high = frequencies.second;

    int samples = static_cast<int>(duration * FS);
    std::vector<float> signal(samples);

    for (int i = 0; i < samples; i++) {
        double t = static_cast<double>(i) / FS;

        signal[i] = 0.5 * (
            std::sin(2 * PI * f_low * t) +
            std::sin(2 * PI * f_high * t)
        ) / 2.0;
    }

    return signal;
}

// Opretter en sekvens af toner med pauser
std::vector<float> make_signal(
    const std::string& keys,
    double tone_dur = 0.1,
    double gap = 0.1
) {
    std::vector<float> signal;

    int silence_samples = static_cast<int>(gap * FS);

    for (char key : keys) {
        std::vector<float> samples = tone(key, tone_dur);

        signal.insert(
            signal.end(),
            samples.begin(),
            samples.end()
        );

        signal.insert(
            signal.end(),
            silence_samples,
            0.0f
        );
    }

    return signal;
}

// Afspiller lydsekvensen med PortAudio
void play_sequence(
    const std::string& keys,
    double tone_dur = 0.05,
    double gap = 0.1
) {
    std::vector<float> signal =
        make_signal(keys, tone_dur, gap);

    PaError err = Pa_Initialize();

    if (err != paNoError) {
        std::cerr << "Fejl ved initialisering af PortAudio\n";
        return;
    }

    PaStream* stream = nullptr;

    err = Pa_OpenDefaultStream(
        &stream,
        0,              // Ingen inputkanaler
        1,              // Mono output
        paFloat32,      // Float-format
        FS,             // Samplingsfrekvens: 8000 Hz
        paFramesPerBufferUnspecified,
        nullptr,
        nullptr
    );

    if (err != paNoError) {
        std::cerr << "Kunne ikke åbne lydstream\n";
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);

    if (err == paNoError) {
        err = Pa_WriteStream(
            stream,
            signal.data(),
            static_cast<unsigned long>(signal.size())
        );
    }

    if (err != paNoError) {
        std::cerr << "Fejl ved afspilning af lyd\n";
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
}

int main() {
    play_sequence("3", 0.04, 0.4);
    return 0;
}
