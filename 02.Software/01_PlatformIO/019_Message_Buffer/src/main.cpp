#include <Arduino.h>
#include <freertos/message_buffer.h>
#include "decoder/decoder.h"

MessageBufferHandle_t xMessageBuffer;
TickType_t timeout = 200;

void readGPS(void *param)
{
  String gpsInfo;
  size_t xBytesSent;

  while (1)
  {
    vTaskDelay(500);
    xBytesSent = xMessageBufferSend(xMessageBuffer, (void *)&gpsInfo, sizeof(gpsInfo), timeout);

    if (xBytesSent != sizeof(gpsInfo))
    {
      Serial.println("Send Error !");
    }
    vTaskDelay(10);
  }
}

void showGPS(void *param)
{
  size_t xMessageSizeMax = 100;
  String gpsInfo;
  size_t xBytesReceive;

  while (1)
  {
    xBytesReceive = xMessageBufferReceive(xMessageBuffer, (void *)&gpsInfo, xMessageSizeMax, timeout);

    if (xBytesReceive > 0)
    {
      gpsDecoder(gpsInfo);
    }
    vTaskDelay(10);
  }
}

void monitorGPS(void *param)
{
  size_t xAvailable, xUsed;
  bool isFull;
  while (1)
  {
    // Queries a stream buffer to see if it is full.
    if (xMessageBufferIsFull(xMessageBuffer) == pdTRUE)
      Serial.println("xMessageBuffer is Full");

    // Queries a stream buffer to see how much free space it contains
    xAvailable = xMessageBufferSpacesAvailable(xMessageBuffer);

    char msg[40];
    sprintf(msg, "xMessageBuffer available space %d bytes", xAvailable);
    Serial.println(msg);

    vTaskDelay(1000);
  }
}
void setup()
{
  Serial.begin(115200);

  const size_t xMessageBufferSizeBytes = 100;
  xMessageBuffer = xMessageBufferCreate(xMessageBufferSizeBytes);

  if (xMessageBuffer == NULL)
  {
    Serial.println("Unable to Create Message Buffer");
  }
  else
  {
    xTaskCreate(readGPS, "Read GPX", 1024 * 4, NULL, 1, NULL);
    xTaskCreate(showGPS, "Show GPX", 1024 * 4, NULL, 1, NULL);
    xTaskCreate(monitorGPS, "Monitor", 1024 * 8, NULL, 1, NULL); 
  }
}

void loop()
{
}
