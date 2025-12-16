#!/usr/bin/env bash
# Make an MP4 video from ParticleViewer image frames.
# Usage: ./make_video.sh [frame_pattern] [output_mp4]
# Defaults: frames=outputs/view_%03d.png, video=outputs/particle_video.mp4

set -euo pipefail

frames=${1:-outputs/view_%03d.png}
video=${2:-outputs/particle_video.mp4}

if ! command -v ffmpeg >/dev/null 2>&1; then
  echo "ffmpeg not found. Install it (e.g., sudo apt-get install ffmpeg)." >&2
  exit 1
fi

mkdir -p "$(dirname "$video")"

echo "Making video from frames: $frames"

# Basic existence check: translate %03d to * for probing
probe_pattern="$frames"
probe_pattern=${probe_pattern//%0*d/*}
probe_pattern=${probe_pattern//%d/*}
if ! compgen -G "$probe_pattern" > /dev/null; then
  echo "No frames found matching: $probe_pattern" >&2
  exit 2
fi

# Use printf-style pattern (no glob) so zero-padded sequences work
ffmpeg -y -framerate 30 -i "$frames" \
  -c:v libx264 -pix_fmt yuv420p "$video"

echo "Done: $video"
