import sys
import matplotlib.pyplot as plt
from collections import deque
import argparse


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--samples",
        type=int,
        default=300,
        help="Number of displayed measurements"
    )

    parser.add_argument(
        "--max-amplitude",
        type=float,
        default=0.6,
        help="Maximum y-axis amplitude"
    )

    args = parser.parse_args()

    values = deque(
        [0.0] * args.samples,
        maxlen=args.samples
    )

    plt.ion()

    fig, ax = plt.subplots()

    line, = ax.plot(
        range(args.samples),
        values
    )

    ax.set_xlim(0, args.samples - 1)
    ax.set_ylim(0, args.max_amplitude)

    ax.set_xlabel("Measurement")
    ax.set_ylabel("RMS amplitude")
    ax.set_title("Raspberry Pi microphone")

    ax.grid(True)

    try:
        for input_line in sys.stdin:

            input_line = input_line.strip()

            if not input_line:
                continue

            try:
                value = float(input_line)
            except ValueError:
                continue

            values.append(value)

            line.set_ydata(values)

            fig.canvas.draw_idle()
            fig.canvas.flush_events()

    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()