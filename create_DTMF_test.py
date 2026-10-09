import numpy as np
import sounddevice as sd

FS = 48000  

DTMF1 = {
    '1': (697, 1209), '2': (697, 1336), '3': (697, 1477), 'A': (697, 1633),
    '4': (770, 1209), '5': (770, 1336), '6': (770, 1477), 'B': (770, 1633),
    '7': (852, 1209), '8': (852, 1336), '9': (852, 1477), 'C': (852, 1633),
    '*': (941, 1209), '0': (941, 1336), '#': (941, 1477), 'D': (941, 1633),
}

DTMF2 = {
    '1': (1697, 2209), '2': (1697, 2336), '3': (1697, 2477), 'A': (1697, 2633),
    '4': (1770, 2209), '5': (1770, 2336), '6': (1770, 2477), 'B': (1770, 2633),
    '7': (1852, 2209), '8': (1852, 2336), '9': (1852, 2477), 'C': (1852, 2633),
    '*': (1941, 2209), '0': (1941, 2336), '#': (1941, 2477), 'D': (1941, 2633),
}

def tone(key, duration=0.1, fs=FS):
    f_low, f_high = DTMF2[key]
    t = np.arange(int(duration * fs)) / fs
    return 0.5 * (np.sin(2*np.pi*f_low*t) + np.sin(2*np.pi*f_high*t)) / 2

def make_signal(keys, tone_dur=0.1, gap=0.1):
    silence = np.zeros(int(gap * FS))
    return np.concatenate([np.concatenate((tone(k, tone_dur), silence)) for k in keys])

def play_sequence(keys, tone_dur=2.1, gap=1.1):
    sd.play(make_signal(keys, tone_dur, gap), FS)
    sd.wait()

if __name__ == "__main__":
    play_sequence("9")