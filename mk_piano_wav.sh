#!/usr/bin/sh
mkdir piano-wav
for i in piano-mp3/*.mp3
do
    ffmpeg -i $i piano-wav/`basename -s .mp3 $i`.wav
done
