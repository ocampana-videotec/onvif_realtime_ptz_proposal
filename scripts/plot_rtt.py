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
                "id": int(row["id"]),
                "webrtc_rtt_ns": int(row["webrtc_rtt_ns"]),
                "end_to_end_rtt_ns": int(row["end_to_end_rtt_ns"]),
            })
    return rows


def main():
    parser = argparse.ArgumentParser(description="Plot RTT statistics from CSV.")
    parser.add_argument("--csv", default="rtt_stats.csv", help="Path to CSV file")
    parser.add_argument("--out", default="rtt_plot.png", help="Output image path")
    args = parser.parse_args()

    csv_path = Path(args.csv)
    if not csv_path.exists():
        raise SystemExit(f"CSV not found: {csv_path}")

    rows = read_csv(csv_path)
    if not rows:
        raise SystemExit("CSV has no rows to plot")

    ids = [row["id"] for row in rows]
    webrtc_ms = [row["webrtc_rtt_ns"] / 1e6 for row in rows]
    end_to_end_ms = [row["end_to_end_rtt_ns"] / 1e6 for row in rows]

    plt.figure(figsize=(10, 5))
    plt.plot(ids, webrtc_ms, label="WebRTC RTT (ms)")
    plt.plot(ids, end_to_end_ms, label="End-to-end RTT (ms)")
    plt.xlabel("Message ID")
    plt.ylabel("RTT (ms)")
    plt.title("RTT Statistics")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(args.out)
    print(f"Wrote plot to {args.out}")


if __name__ == "__main__":
    main()
