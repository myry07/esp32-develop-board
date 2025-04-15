#include <Arduino.h>
#include <freertos/stream_buffer.h>
#include "decoder.h"

StreamBufferHandle_t xStreamMusic;

void downloadTask(void *pvParam)
{
  String music;
  size_t xBytesSent; // The number of bytes written to the stream buffer.

  while (1)
  {
    vTaskDelay(500);
    music = randomMusic();
    xBytesSent = xStreamBufferSend(xStreamMusic, (void *)&music, sizeof(music), portMAX_DELAY);

    if (xBytesSent != sizeof(music))
    {
      Serial.println("Size Error");
    }
    vTaskDelay(100);
  }
}

void playbackTask(void *param)
{
  size_t xReceivedBytes; // The number of bytes read from the stream buffer.
  size_t xReadBytes = 8 * 10 - 1;
  String music;

  while (1)
  {
    xReceivedBytes = xStreamBufferReceive(xStreamMusic, (void *)&music, xReadBytes, portMAX_DELAY);
    if (xReceivedBytes > 0)
    {
      decode(music);
    }
  }
}

void monitorTask(void *pvParam)
{
  size_t xAvailable, xUsed;
  bool isFull;
  while (1)
  {

    // Queries a stream buffer to see if it is full.
    if (xStreamBufferIsFull(xStreamMusic) == pdTRUE)
      Serial.println("xStreamMusic is Full");

    // Queries a stream buffer to see how much data it contains
    xUsed = xStreamBufferBytesAvailable(xStreamMusic);

    // Queries a stream buffer to see how much free space it contains
    xAvailable = xStreamBufferSpacesAvailable(xStreamMusic);

    char msg[40];
    sprintf(msg, "xStreamBuffer used: %d byte", xUsed);
    Serial.println(msg);
    sprintf(msg, "xStreamBuffer available space %d byte", xAvailable);
    Serial.println(msg);

    vTaskDelay(2000);
  }
}

void setup()
{
  Serial.begin(115200);

  const size_t xStreamBufferSizeBytes = 540;
  const size_t xTriggerLevel = 8;
  xStreamMusic = xStreamBufferCreate(xStreamBufferSizeBytes, xTriggerLevel);

  xTaskCreate(downloadTask, "Download Music", 1024 * 8, NULL, 1, NULL);
  xTaskCreate(playbackTask, "Playback Music", 1024 * 8, NULL, 1, NULL);
  xTaskCreate(monitorTask, "Monitor Stream Buffer", 1024 * 8, NULL, 1, NULL);
}

void loop()
{
}
