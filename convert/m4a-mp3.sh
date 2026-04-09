#!/bin/bash

for f in *.m4a; do ffmpeg -i "$f" -c:a libmp3lame -q:a 2 "${f%.m4a}.mp3"; done
