#ifndef FadeLED_h
#define FadeLED_h

#include <FastLED.h>

#define NUM_LEDS 10
#define DATA_PIN 25
#define QUEUE_LENGTH 20

struct FadeQueue {
  CRGB startRGB;
  CRGB targetRGB;
  bool useStartColor;
  uint16_t fadeTransitionSteps;
  uint16_t fadeCountdown;
};

struct FadeLEDs {
  CRGB currentRGB;
  CRGB targetRGB;
  CRGB stepRGB;
  uint8_t queueTail;
  uint8_t queueHead;
  uint8_t freeQueueElements;
  uint16_t fadeStep;
  uint16_t fadeCountdown;
  uint16_t fadeTransitionSteps;
  bool fadeCompleted;
  FadeQueue fadeQueue[QUEUE_LENGTH];
};

class FadeLED {
  public:
    FadeLED();
	CRGB leds[NUM_LEDS];
	FadeLEDs fadeLEDs[NUM_LEDS];
    float globalBrightFactor;
    void update(void);
    void brightness(float brightness);
    void addFadeToQueue(FadeQueue d, uint8_t led) ;
    bool allQueuesEmpty();
    bool queueEmpty(uint8_t led);

  private:
    void applyNextQueueItem(uint8_t number);
    void updateLEDFade(uint8_t number);
    void setPhysicalLED(uint8_t number);
    void updateLED(uint8_t number);
    void initialiseLEDs(void);

};

extern FadeLED fade;

#endif
