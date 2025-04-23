# esp32-develop-board

mp4 -> mp3  
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


the 1.0 Version of the board
<div align=center>
	<img src="https://github.com/myry07/esp32-develop-board/blob/main/05.Fotos/1.jpg" width="600" height="450">
</div>