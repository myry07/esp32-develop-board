#include <Arduino.h>
#include "string.h"
#include "decoder.h"

#define BUZZER_PIN 32
#define BUZZER_CHANNEL 0

 typedef struct {
  int noteVal;
  int octaveVal;
  int restVal;
} BUZZERTONE;

//随机返回生成的音乐代码，每次的长度不一样
String randomMusic() {
  String randomNote;
  randomSeed(analogRead(34));
  for (int i = 0; i < random(5, 20); i++) {
    char note[10];
    sprintf(note, "%d,%d,%d", random(0, 9), random(3, 6), random(100, 999));
    randomNote = randomNote + "-" + String(note);
  }
  randomNote = randomNote.substring(1, randomNote.length());
  return randomNote;
}

//对音乐流进行解码后，通过buzzer放出来
void decode(String music) {
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  do {
    int index = music.indexOf('-');
    String val = music.substring(0, index);
    BUZZERTONE buzzertone;
    buzzertone.noteVal = val.substring(0, 1).toInt();
    buzzertone.octaveVal = val.substring(2, 3).toInt();
    buzzertone.restVal = val.substring(4, 7).toInt();
    ledcWriteNote(BUZZER_CHANNEL, (note_t)buzzertone.noteVal, buzzertone.octaveVal);
    vTaskDelay(buzzertone.restVal);
    if (music.indexOf('-') < 0) music = "";
    music = music.substring(index + 1, music.length());
  } while (music.length() > 0);
  ledcDetachPin(BUZZER_PIN);
}