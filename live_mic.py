import sounddevice as sd
import numpy as np
import matplotlib.pyplot as plt

fs = 44100
blocksize = 1024

x = np.zeros(blocksize)

plt.ion()
fig, ax = plt.subplots()

line, = ax.plot(x)
ax.set_ylim(-1, 1)
ax.set_xlim(0, blocksize)
ax.set_xlabel("Sample")
ax.set_ylabel("Amplitude")
ax.set_title("Live mikrofon")

def callback(indata, frames, time, status):
    global x
    x = indata[:, 0].copy()

with sd.InputStream(
    channels=1,
    samplerate=fs,
    blocksize=blocksize,
    callback=callback
):
    while plt.fignum_exists(fig.number):
        line.set_ydata(x)
        fig.canvas.draw_idle()
        fig.canvas.flush_events()
        plt.pause(0.01)