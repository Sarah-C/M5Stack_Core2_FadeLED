#include <M5Core2.h>
#include "FadeLED.h"

int stage = 0;

void setup() {
  M5.begin();
  randomSeed(analogRead(0));
  fade.brightness(1);
  xTaskCreatePinnedToCore(updateDisplay, "Update display.", 1000, NULL, 1, NULL, 1);
}

void loop(void) {
  M5.update();
  fade.update();
  delay(10);
  maybeAddNewFadyStage();
}

void addFade(uint8_t r, uint8_t g, uint8_t b, uint16_t fadeCountdown, uint16_t transitionSteps, uint8_t led) {
  FadeQueue fq;
  fq.startRGB = CRGB(0, 0, 0);
  fq.targetRGB = CRGB(r, g, b);
  fq.useStartColor = false;
  fq.fadeTransitionSteps = transitionSteps;
  fq.fadeCountdown = fadeCountdown;
  fade.addFadeToQueue(fq, led);
}

void maybeAddNewFadyStage() {
  if (fade.allQueuesEmpty()) {
    // Green swish...
    if (stage == 0 || stage == 2) {
      for (int i = 0; i < 10; i++) {
        addFade(0, 255, 0, 5 + (5 * i), 3, i);
        addFade(128, 255, 128, 0, 1, i);
        addFade(0, 255, 0, 0, 1, i);
        addFade(0, 0, 0, 0, 15, i);
      }
      stage++;
      return;
    }
    // Red swish...
    if (stage == 1 || stage == 3) {
      for (int i = 0; i < 10; i++) {
        addFade(255, 0, 0, 5 + (5 * i), 3, 9 - i);
        addFade(255, 128, 128, 0, 1, 9 - i);
        addFade(255, 0, 0, 0, 1, 9 - i);
        addFade(0, 0, 0, 0, 15, 9 - i);
      }
      stage++;
      return;
    }
    // Multicoloured rug...
    if (stage == 4) {
      for (int i = 0; i < 90; i++) {
        uint8_t led = random(0, 10);
        addFade(random(0, 255), random(0, 255), random(0, 255),    random(1, 20),   random(5, 20), led);
        addFade(150, 0, 0, 0, 10, led);
      }
      stage++;
      return;
    }
    // Camera flashes...
    if (stage == 5) {
      for (int i = 0; i < 150; i++) {
        uint8_t led = random(0, 10);
        addFade(255, 255, 255, random(1, 50), 1, led);
        addFade(0, 0, 0, 0, 1, led);
      }
      stage ++;
      return;
    }
    // Police flashers...
    if (stage < 15) {
      for (int led = 0; led < 5; led++) {
        addFade(255, 0, 0, 26, 1, led);
        addFade(0, 0, 0, 0, 1, led);
        addFade(255, 0, 0, 7, 1, led);
        addFade(0, 0, 0, 0, 1, led);
      }
      for (int led = 5; led < 10; led++) {
        addFade(0, 0, 0, 13, 1, led);
      }
      for (int led = 5; led < 10; led++) {
        addFade(0, 0, 255, 26, 1, led);
        addFade(0, 0, 0, 0, 1, led);
        addFade(0, 0, 255, 7, 1, led);
        addFade(0, 0, 0, 0, 1, led);
      }
      stage ++;
      return;
    }
    stage = 0;
  }
}
//##########################################

// Task that runs independently showing on the M5Stack Core2 screen what the LED's are displaying.
void updateDisplay(void * parameter) {
  const uint16_t size = 15;
  const uint16_t xo = 160 - ((10 * size) >> 1);
  for (;;) { // infinite loop
    for (uint8_t i = 0; i < 10; i++) {
      int y = (i < 5) ? i * 48 : 240 - ((i - 4) * 48);
      int x = (i < 5) ? (320 - 50) : 0;
      uint16_t r = round(fade.fadeLEDs[i].currentRGB.r);
      uint16_t g = round(fade.fadeLEDs[i].currentRGB.g);
      uint16_t b = round(fade.fadeLEDs[i].currentRGB.b);
      uint16_t col = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
      M5.Lcd.fillRect(x, y, 50, 48, col);
      M5.Lcd.fillRect(xo + (i * size), 90, size - 2, size - 2, col);
    }
    // Pause the task
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}