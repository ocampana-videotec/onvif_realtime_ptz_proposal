#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def read_csv(path: Path):
    rows = []
    with path.open(newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append({
                "webrtc_rtt_ns": int(row["webrtc_rtt_ns"]),
                "end_to_end_rtt_ns": int(row["end_to_end_rtt_ns"]),
            })
    return rows


def main():
    parser = argparse.ArgumentParser(description="Plot RTT histograms from CSV.")
    parser.add_argument("--csv", default="rtt_stats.csv", help="Path to CSV file")
    parser.add_argument("--out", default="rtt_hist.png", help="Output image path")
    parser.add_argument("--bins", type=int, default=40, help="Number of histogram bins")
    parser.add_argument(
        "--metric",
        choices=["webrtc", "end_to_end", "both"],
        default="both",
        help="Which RTT metric(s) to plot",
    )
    args = parser.parse_args()

    csv_path = Path(args.csv)
    if not csv_path.exists():
        raise SystemExit(f"CSV not found: {csv_path}")

    rows = read_csv(csv_path)
    if not rows:
        raise SystemExit("CSV has no rows to plot")

    webrtc_ms = [row["webrtc_rtt_ns"] / 1e6 for row in rows]
    end_to_end_ms = [row["end_to_end_rtt_ns"] / 1e6 for row in rows]

    plt.figure(figsize=(8, 5))
    if args.metric in ("webrtc", "both"):
        plt.hist(webrtc_ms, bins=args.bins, alpha=0.6, label="WebRTC RTT (ms)")
    if args.metric in ("end_to_end", "both"):
        plt.hist(end_to_end_ms, bins=args.bins, alpha=0.6, label="End-to-end RTT (ms)")

    plt.xlabel("RTT (ms)")
    plt.ylabel("Count")
    plt.title("RTT Histogram")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(args.out)
    print(f"Wrote histogram to {args.out}")


if __name__ == "__main__":
    main()
