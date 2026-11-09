
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <portaudio.h>

const int FS = 16000;
const double PI = 3.14159265358979323846;

const std::map<int, std::pair<double, double>> DTMF1 = {
    {1,  {697, 1209}}, {2,  {697, 1336}},
    {3,  {697, 1477}}, {4,  {697, 1633}},
    {5,  {770, 1209}}, {6,  {770, 1336}},
    {7,  {770, 1477}}, {8,  {770, 1633}},
    {9,  {852, 1209}}, {10, {852, 1336}},
    {11, {852, 1477}}, {12, {852, 1633}},
    {13, {941, 1209}}, {14, {941, 1336}},
    {15, {941, 1477}}, {16, {941, 1633}}
};

// Generate a DTMF tone
std::vector<float> tone(int key, double duration = 0.1) {

    auto frequencies = DTMF1.at(key);

    double f_low = frequencies.first;
    double f_high = frequencies.second;

    int samples = static_cast<int>(duration * FS);

    std::vector<float> signal(samples);

    for (int i = 0; i < samples; i++) {

        double t = static_cast<double>(i) / FS;

        signal[i] = 0.25f * (
            std::sin(2 * PI * f_low * t) +
            std::sin(2 * PI * f_high * t)
        );
    }

    return signal;
}

// Generate a sequence of DTMF tones with gaps
std::vector<float> make_signal(
    const std::vector<int>& keys,
    double tone_dur = 0.1,
    double gap = 0.1
) {

    std::vector<float> signal;

    int silence_samples = static_cast<int>(gap * FS);

    for (int key : keys) {

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

// Play sequence using PortAudio
void play_sequence(
    const std::vector<int>& keys,
    double tone_dur = 0.05,
    double gap = 0.1
) {

    std::vector<float> signal =
        make_signal(keys, tone_dur, gap);

    // Add 100 ms silence at the beginning
    signal.insert(
        signal.begin(),
        FS / 10,
        0.0f
    );

    // Add 100 ms silence at the end
    signal.insert(
        signal.end(),
        FS / 10,
        0.0f
    );

    PaError err = Pa_Initialize();

    if (err != paNoError) {
        std::cerr << "PortAudio initialization error: "
                  << Pa_GetErrorText(err) << std::endl;
        return;
    }

    PaStream* stream = nullptr;

    err = Pa_OpenDefaultStream(
        &stream,
        0,
        1,
        paFloat32,
        FS,
        paFramesPerBufferUnspecified,
        nullptr,
        nullptr
    );

    if (err != paNoError) {
        std::cerr << "PortAudio stream error: "
                  << Pa_GetErrorText(err) << std::endl;

        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);

    if (err != paNoError) {
        std::cerr << "PortAudio start error: "
                  << Pa_GetErrorText(err) << std::endl;

        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }

    // Write all audio samples
    err = Pa_WriteStream(
        stream,
        signal.data(),
        static_cast<unsigned long>(signal.size())
    );

    if (err != paNoError) {
        std::cerr << "PortAudio write error: "
                  << Pa_GetErrorText(err) << std::endl;
    }

    // Stop stream gracefully
    PaError stopErr = Pa_StopStream(stream);

    if (stopErr != paNoError) {
        std::cerr << "PortAudio stop error: "
                  << Pa_GetErrorText(stopErr) << std::endl;
    }

    // Close stream
    Pa_CloseStream(stream);

    // Terminate PortAudio
    Pa_Terminate();
}

int main() {

    srand(static_cast<unsigned int>(time(nullptr)));

    // Generate sequence starting with the number set below, followed by 5 random numbers between 1-16
    std::vector<int> sequence = {2};

    for (int i = 1; i < 6; i++) {
        sequence.push_back(rand() % 16 + 1);
    }

    // Print generated sequence
    std::cout << "Generated sequence: ";

    for (int i = 0; i < sequence.size(); i++) {
        std::cout << sequence[i];

        if (i < sequence.size() - 1) {
            std::cout << ", ";
        }
    }

    std::cout << std::endl;

    // Play sequence
    play_sequence(sequence, 0.03, 0.04);

    return 0;
}
