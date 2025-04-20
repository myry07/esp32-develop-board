# esp32-develop-board

mp4 - > mp3\n
sample rate 44100Hz\n
single channel\n
quality 3\n

```ffmpeg -i test.mp4 -ar 44100 -ac 1 -q:a 3 test.mp3```

mp3 -> pcm\n
sample bit 16 bits\n
sample rate 44100Hz\n

```ffmpeg -i test.mp3 -f s16le -ar 44100 -ac 1 -acodec pcm_s16le test.pcm```