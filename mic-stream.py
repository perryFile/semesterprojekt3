# mic_stream.py

import sounddevice as sd
import argparse
import sys

parser = argparse.ArgumentParser()

parser.add_argument("--device", type=int, default=None)
parser.add_argument("--samplerate", type=int, default=44100)
parser.add_argument("--blocksize", type=int, default=1024)

args = parser.parse_args()


def callback(indata, frames, time, status):
    if status:
        print(status, file=sys.stderr)

    samples = indata[:, 0]

    # Send hele blokken som én linje
    print(" ".join(f"{x:.6f}" for x in samples), flush=True)


with sd.InputStream(
    device=args.device,
    channels=1,
    samplerate=args.samplerate,
    blocksize=args.blocksize,
    dtype="float32",
    callback=callback
):
    try:
        while True:
            sd.sleep(1000)
    except KeyboardInterrupt:
        pass