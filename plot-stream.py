# plot-stream.py

import sys
import numpy as np
import matplotlib.pyplot as plt

FS = 44100
BLOCKSIZE = 1024

plt.ion()

fig, ax = plt.subplots()

x = np.arange(BLOCKSIZE) / FS * 1000  # ms
y = np.zeros(BLOCKSIZE)

line, = ax.plot(x, y)

ax.set_xlim(0, x[-1])
ax.set_ylim(-1, 1)

ax.set_xlabel("Time [ms]")
ax.set_ylabel("Amplitude")
ax.set_title("Live microphone waveform")

ax.grid(True)

for stdin_line in sys.stdin:

    try:
        samples = np.fromstring(stdin_line, sep=" ")

        if len(samples) == 0:
            continue

        # Hvis blocksize ændrer sig
        if len(samples) != len(x):
            x = np.arange(len(samples)) / FS * 1000

            line.set_xdata(x)
            ax.set_xlim(0, x[-1])

        line.set_ydata(samples)

        fig.canvas.draw_idle()
        fig.canvas.flush_events()

    except KeyboardInterrupt:
        break