#!/bin/bash

for f in *.wav; do ffmpeg -i "$f" -c:a libmp3lame -q:a 2 "${f%.wav}.mp3"; done
