#include <M5Core2.h>
#include "FadeLED.h"

uint8_t stage = 0;
uint8_t oldStage = 1; // For knowing when to draw the flash description on the screen.

void setup() {
  M5.begin();
  randomSeed(analogRead(0));
  fade.setBrightness(200);
  xTaskCreatePinnedToCore(updateDisplayTask, "Update display.", 1000, NULL, 1, NULL, 1);
}

void updateDisplay(void);

void loop(void) {
  M5.update();
  fade.update();
  delay(10);
  maybeAddNewFadyStage();
}

void addFade(uint8_t r, uint8_t g, uint8_t b, uint16_t fadeCountdown, uint16_t fadeCountup, uint16_t transitionSteps, uint8_t led) {
  FadeQueue fq;
  fq.startRGB = CRGB(0, 0, 0);
  fq.targetRGB = CRGB(r, g, b);
  fq.useStartColor = false;
  if (transitionSteps == 0) transitionSteps = 1;
  fq.fadeTransitionSteps = transitionSteps;
  fq.fadeCountdown = fadeCountdown;
  fq.fadeCountup = fadeCountup;
  fade.addFadeToQueue(fq, led);
}

void maybeAddNewFadyStage() {
  if (fade.allQueuesEmpty()) {
    // Green swish...
    if (stage == 0 || stage == 2) {
      for (int i = 0; i < 10; i++) {
        addFade(0, 255, 0, 5 + (5 * i), 0, 3, i);
        addFade(128, 255, 128, 0, 0, 1, i);
        addFade(0, 255, 0, 0, 0, 1, i);
        addFade(0, 0, 0, 0, 0, 15, i);
      }
      stage++;
      return;
    }
    // Red swish...
    if (stage == 1 || stage == 3) {
      // M5.Lcd.fillScreen(TFT_BLACK);
      //M5.Lcd.drawString("Red swish", 160, 145, 4);
      for (int i = 0; i < 10; i++) {
        addFade(255, 0, 0, 5 + (5 * i), 0, 3, 9 - i);
        addFade(255, 128, 128, 0, 0, 1, 9 - i);
        addFade(255, 0, 0, 0, 0,  1, 9 - i);
        addFade(0, 0, 0, 0, 0, 15, 9 - i);
      }
      stage++;
      return;
    }
    // Multicoloured rug...
    if (stage == 4) {
      //M5.Lcd.fillScreen(TFT_BLACK);
      //M5.Lcd.drawString("Patchwork quilt", 160, 145, 4);
      for (int i = 0; i < 90; i++) {
        uint8_t led = random(0, 10);
        addFade(random(0, 255), random(0, 255), random(0, 255),    random(1, 20), 0,  random(5, 20), led);
        addFade(150, 0, 0, 0, 0, 10, led);
      }
      stage++;
      return;
    }
    // Camera flashes...
    //M5.Lcd.fillScreen(TFT_BLACK);
    //M5.Lcd.drawString("Camera flashes", 160, 145, 4);
    if (stage == 5) {
      for (int i = 0; i < 150; i++) {
        uint8_t led = random(0, 10);
        addFade(255, 255, 255, random(1, 50), 0, 1, led);
        addFade(0, 0, 0, 0, 0, 1, led);
      }
      stage ++;
      return;
    }
    // Police flashers...
    if (stage >= 6 && stage < 15) {
      // M5.Lcd.fillScreen(TFT_BLACK);
      //M5.Lcd.drawString("Police flasher", 160, 145, 4);
      for (int led = 0; led < 5; led++) {
        addFade(255, 0, 0, 26, 0, 1, led);
        addFade(0, 0, 0, 0, 0, 1, led);
        addFade(255, 0, 0, 7, 0, 1, led);
        addFade(0, 0, 0, 0, 0, 1, led);
      }
      for (int led = 5; led < 10; led++) {
        addFade(0, 0, 0, 13, 0, 1, led);
      }
      for (int led = 5; led < 10; led++) {
        addFade(0, 0, 255, 26, 0, 1, led);
        addFade(0, 0, 0, 0, 0, 1, led);
        addFade(0, 0, 255, 7, 0, 1, led);
        addFade(0, 0, 0, 0, 0, 1, led);
      }
      stage ++;
      return;
    }
    // Warning orange flasher
    if (stage >= 15 && stage < 19) {
      //  M5.Lcd.fillScreen(TFT_BLACK);
      //M5.Lcd.drawString("Warning light", 160, 145, 4);
      for (int i = 0; i < 10; i++) {
        addFade(255, 128, 0,   0, 0,    100, i);
        addFade(0, 0, 0,       0, 25,   1,   i);
        addFade(255, 128, 0,   0, 25,   1,   i);
        addFade(0, 0, 0,       0, 25,   1,   i);
        addFade(255, 128, 0,   0, 25,   1,   i);
        addFade(0, 0, 0,       0, 25,   1,   i);
        addFade(255, 128, 0,   0, 25,   1,   i);
        addFade(0, 0, 0,       0, 25,   1,   i);
        addFade(255, 128, 0,   0, 25,   1,   i);
        addFade(0, 0, 0,       0, 0,    100, i);
      }
      stage++;
      return;
    }
    stage = 0;
  }
}


//##########################################

// Task that runs independently showing on the M5Stack Core2 screen what the LED's are displaying.
void updateDisplayTask(void * parameter) {
  // Writing on the main thread when a task's writing to the LCD is a really bad idea, (multiple LCD writes concurrently)
  // So all the text updates have been moved from the maybeAddNewFadyStage() function to here.
  M5.Lcd.setTextDatum(MC_DATUM); // Middle center align
  M5.Lcd.setTextColor(TFT_WHITE);
  // Configure with this.
  const uint16_t miniBoxSize = 25;
  const uint16_t miniBoxSizeLessBorder = miniBoxSize - 2;
  const uint16_t bigSquareWidth = 25;
  // Other calcs.
  const uint16_t reverseCenterHorizontalBar = miniBoxSize * 9;
  const uint16_t centerHorizontalBar = 160 - ((10 * miniBoxSize) >> 1);
  const uint16_t horBarOffets = centerHorizontalBar + reverseCenterHorizontalBar;
  for (;;) { // infinite loop
    uint8_t s = stage - 1;
    if (oldStage != s) {
      M5.Lcd.fillRect(50, 135, 220, 32, 0);
      oldStage = s;
      if (s == 0 || s == 2) M5.Lcd.drawString("Green swish", 160, 150, 4);
      if (s == 1 || s == 3) M5.Lcd.drawString("Red swish", 160, 150, 4);
      if (s == 4) M5.Lcd.drawString("Patchwork quilt", 160, 150, 4);
      if (s == 5) M5.Lcd.drawString("Camera flashes", 160, 150, 4);
      if (s >= 6 && s < 15) M5.Lcd.drawString("Police flashes", 160, 150, 4);
      if (s >= 15 && s < 19) M5.Lcd.drawString("Warning flashes", 160, 150, 4);
    }
    for (uint8_t i = 0; i < 10; i++) {
      int y = (i < 5) ? i * 48 : 240 - ((i - 4) * 48);
      int x = (i < 5) ? (320 - bigSquareWidth) : 0;
      uint16_t r = round(fade.fadeLEDs[i].currentRGB.r);
      uint16_t g = round(fade.fadeLEDs[i].currentRGB.g);
      uint16_t b = round(fade.fadeLEDs[i].currentRGB.b);
      uint16_t col = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
      M5.Lcd.fillRect(x, y, bigSquareWidth, 48, col);
      M5.Lcd.fillRect(horBarOffets - (i * miniBoxSize), 90, miniBoxSizeLessBorder, miniBoxSizeLessBorder, col);
    }
    // Pause the task
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
