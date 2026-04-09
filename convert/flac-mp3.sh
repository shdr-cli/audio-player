#!/bin/bash

for f in *.flac; do ffmpeg -i "$f" -c:a libmp3lame -q:a 2 "${f%.flac}.mp3"; done
