
#include "FadeLED.h"

CRGB leds[NUM_LEDS];
FadeLEDs fadeLEDs[NUM_LEDS];

FadeLED::FadeLED() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  this->initialiseLEDs();
}

void FadeLED::applyNextQueueItem(uint8_t number) {
  uint8_t tail = fadeLEDs[number].queueTail;
  if (fadeLEDs[number].fadeQueue[tail].useStartColor) {
    fadeLEDs[number].currentRGB = fadeLEDs[number].fadeQueue[tail].startRGB;
  }
  fadeLEDs[number].targetRGB = fadeLEDs[number].fadeQueue[tail].targetRGB;
  fadeLEDs[number].fadeTransitionSteps = fadeLEDs[number].fadeQueue[tail].fadeTransitionSteps;
  fadeLEDs[number].fadeStep = 0;
  fadeLEDs[number].fadeCompleted = false;
  fadeLEDs[number].fadeCountdown = fadeLEDs[number].fadeQueue[tail].fadeCountdown;
  fadeLEDs[number].fadeCountup = fadeLEDs[number].fadeQueue[tail].fadeCountup;
  fadeLEDs[number].stepRGB.r = (fadeLEDs[number].targetRGB.r - fadeLEDs[number].currentRGB.r) / fadeLEDs[number].fadeTransitionSteps;
  fadeLEDs[number].stepRGB.g = (fadeLEDs[number].targetRGB.g - fadeLEDs[number].currentRGB.g) / fadeLEDs[number].fadeTransitionSteps;
  fadeLEDs[number].stepRGB.b = (fadeLEDs[number].targetRGB.b - fadeLEDs[number].currentRGB.b) / fadeLEDs[number].fadeTransitionSteps;
  fadeLEDs[number].freeQueueElements += 1;
  fadeLEDs[number].queueTail += 1;
  if (fadeLEDs[number].queueTail == QUEUE_LENGTH) fadeLEDs[number].queueTail = 0;
}

void FadeLED::updateLEDFade(uint8_t number) {
  if (fadeLEDs[number].fadeStep == fadeLEDs[number].fadeTransitionSteps) {
    // Ensure floating point errors are corrected on the last step by setting exactly to the target.
    fadeLEDs[number].currentRGB = fadeLEDs[number].targetRGB;
    fadeLEDs[number].fadeCompleted = true;
  } else {
    fadeLEDs[number].fadeStep += 1;
    fadeLEDs[number].currentRGB.r += fadeLEDs[number].stepRGB.r;
    fadeLEDs[number].currentRGB.g += fadeLEDs[number].stepRGB.g;
    fadeLEDs[number].currentRGB.b += fadeLEDs[number].stepRGB.b;
  }
}

void FadeLED::setPhysicalLED(uint8_t number) {
  float r = fadeLEDs[number].currentRGB.r;
  float g = fadeLEDs[number].currentRGB.g;
  float b = fadeLEDs[number].currentRGB.b;
  if (r < 0) r = 0;
  if (r > 255) r = 255;
  if (g < 0) g = 0;
  if (g > 255) g = 255;
  if (b < 0) b = 0;
  if (b > 255) b = 255;
  leds[number].r = round(r);
  leds[number].g = round(g);
  leds[number].b = round(b);
}

void FadeLED::updateLED(uint8_t number) {
  if (fadeLEDs[number].fadeCountdown != 0) {
    fadeLEDs[number].fadeCountdown = fadeLEDs[number].fadeCountdown - 1;
    return;
  }
  if (fadeLEDs[number].fadeCompleted) {
    if (fadeLEDs[number].fadeCountup != 0) {
      fadeLEDs[number].fadeCountup = fadeLEDs[number].fadeCountup - 1;
      return;
    }
    if (fadeLEDs[number].freeQueueElements == QUEUE_LENGTH) return;
    this->applyNextQueueItem(number);
    return;
  }
  this->updateLEDFade(number);
  this->setPhysicalLED(number);
}

void FadeLED::update(void) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    this->updateLED(i);
  }
  FastLED.show();
}

void FadeLED::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

void FadeLED::initialiseLED(uint8_t led) {
  fadeLEDs[led].currentRGB.r = 0;
  fadeLEDs[led].currentRGB.g = 0;
  fadeLEDs[led].currentRGB.b = 0;
  fadeLEDs[led].targetRGB.r = 0;
  fadeLEDs[led].targetRGB.g = 0;
  fadeLEDs[led].targetRGB.b = 0;
  fadeLEDs[led].stepRGB.r = 0;
  fadeLEDs[led].stepRGB.g = 0;
  fadeLEDs[led].stepRGB.b = 0;
  fadeLEDs[led].queueTail = 0; // Next free location
  fadeLEDs[led].queueHead = 0; // Last used location
  fadeLEDs[led].freeQueueElements = QUEUE_LENGTH;
  fadeLEDs[led].fadeStep = 0;
  fadeLEDs[led].fadeTransitionSteps = 0;
  fadeLEDs[led].fadeCountdown = 0;
  fadeLEDs[led].fadeCountup = 0;
  fadeLEDs[led].fadeCompleted = true;
}

void FadeLED::initialiseLEDs(void) {
  for (uint8_t led = 0; led < NUM_LEDS; led++) {
    this->initialiseLED(led);
  }
}

void FadeLED::addFadeToQueue(FadeQueue d, uint8_t led) {
  if (fadeLEDs[led].freeQueueElements == 0) return;
  uint8_t head = fadeLEDs[led].queueHead;
  fadeLEDs[led].fadeQueue[head].startRGB = d.startRGB;
  fadeLEDs[led].fadeQueue[head].targetRGB = d.targetRGB;
  fadeLEDs[led].fadeQueue[head].useStartColor = d.useStartColor;
  fadeLEDs[led].fadeQueue[head].fadeTransitionSteps = d.fadeTransitionSteps;
  fadeLEDs[led].fadeQueue[head].fadeCountdown = d.fadeCountdown;
  fadeLEDs[led].fadeQueue[head].fadeCountup = d.fadeCountup;
  fadeLEDs[led].freeQueueElements -= 1;
  fadeLEDs[led].queueHead += 1;
  if (fadeLEDs[led].queueHead == QUEUE_LENGTH) fadeLEDs[led].queueHead = 0;
}

void FadeLED::clearLed(uint8_t led) {
  this->initialiseLED(led);
}

void FadeLED::clearLeds(void) {
  for (uint8_t led = 0; led < NUM_LEDS; led++) {
    this->initialiseLED(led);
  }
}

bool FadeLED::allQueuesEmpty() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    if (!queueEmpty(i)) return false;
  }
  return true;
}

bool FadeLED::queueEmpty(uint8_t led) {
  return (fadeLEDs[led].freeQueueElements == QUEUE_LENGTH && fadeLEDs[led].fadeCompleted);
}

FadeLED fade;
