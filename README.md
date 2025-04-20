# esp32-develop-board

mp4 - > mp3  
sample rate 44100Hz  
single channel  
quality 3  

```
ffmpeg -i test.mp4 -ar 44100 -ac 1 -q:a 3 test.mp3
```

mp3 -> pcm  
sample bit 16 bits  
sample rate 44100Hz  

```
ffmpeg -i test.mp3 -f s16le -ar 44100 -ac 1 -acodec pcm_s16le test.pcm
```