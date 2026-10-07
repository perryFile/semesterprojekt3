import sounddevice as sd
import numpy as np
import argparse
import sys


def list_devices():
    print(sd.query_devices())


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--device",
        type=int,
        default=None,
        help="Input device index"
    )

    parser.add_argument(
        "--samplerate",
        type=int,
        default=44100,
        help="Sample rate in Hz"
    )

    parser.add_argument(
        "--blocksize",
        type=int,
        default=1024,
        help="Samples per block"
    )

    parser.add_argument(
        "--raw",
        action="store_true",
        help="Output all raw microphone samples instead of RMS"
    )

    parser.add_argument(
        "--list-devices",
        action="store_true",
        help="Show available audio devices"
    )

    args = parser.parse_args()

    if args.list_devices:
        list_devices()
        return

    def callback(indata, frames, time, status):
        if status:
            print(status, file=sys.stderr)

        samples = indata[:, 0]

        if args.raw:
            for sample in samples:
                print(f"{sample:.6f}", flush=True)
        else:
            rms = np.sqrt(np.mean(samples ** 2))
            print(f"{rms:.6f}", flush=True)

    try:
        with sd.InputStream(
            device=args.device,
            channels=1,
            samplerate=args.samplerate,
            blocksize=args.blocksize,
            dtype="float32",
            callback=callback
        ):
            while True:
                sd.sleep(1000)

    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()