# M5Stack Core2: FadeLED

Example: https://youtu.be/tLgJLZ2Suhc

![image](https://user-images.githubusercontent.com/1586332/130353388-cac25ab9-39a8-47e4-86eb-d858d7f193f6.png)

A simple queued LED fader library for the M5Stack - Core2, Arduino.

Similar to https://github.com/Sarah-C/Arduino_GFadeLED/ , but hides all the FastLED code and makes setup a lot simpler, due to being designed for the M5Go2 bottom. 
( https://shop.m5stack.com/products/m5go-battery-bottom2-for-core2-only )

Just drop it in your documents/Arduino/library folder, and then check out the example sketch!

(You'll need the FastLED library too - get it from the Arduino library manager in the tools drop-down menu.)

    // To add a new LED fade effect, you pass the following to fade.addFadeToQueue()
    CRGB startRGB;
    CRGB targetRGB;
    bool useStartColor;
    uint16_t fadeTransitionSteps;
    uint16_t fadeCountdown;
    uint16_t fadeCountup;

    // For example, fade to red, from whatever color the LED is now, without a start pause or end pause, fade takes 50 cycles:
    FadeQueue fqItem;
    uint8_t led = 0;
    fqItem.startRGB = CRGB(0, 0, 0);
    fqItem.targetRGB = CRGB(255, 0, 0);
    fqItem.useStartColor = false;
    fqItem.fadeTransitionSteps = 50;
    fqItem.fadeCountdown = 0;
    fqItem.fadeCountup = 0;
    fade.addFadeToQueue(fqItem, led);
  
  
startRGB = If "useStartColor" is set to true, the LED is set to this color instantly before the fade to targerRGB starts. Not often used.           
targetRGB = This is the color the LED will fade to.             
useStartColor = Normally false, if true the LED as set to startRGB instantly, before fading to targetRGB.            
fadeTransitionSteps = 1 to 255, this is the number of times fade.update() has to be called to fade to targetRGB.             
fadeCountdown = The number of calls to fade.update() BEFORE the LED starts fading its color.
fadeCountup = The number of calls to fade.update() AFTER the LED finishes fading its color before the next item in the queue.

fadeCountdown and fadeCountup are to time the fade easily relative to other leds.
If fadeCountdown + fadeCountup for each pixel are the same for al the pixels involved in a fade, it guarantees the next fade added to the queue will be timed correctly.

For instance, all the leds will light up one following the next, and then all fade out at the same time, due to fadeCountup and fadeCountdown all totalling the same number of cycles:

     for (int led = 0; led < 10; led++) {
        uint8_t pause = led;
        addFade(255, 255, 255,  pause * 10,    10 - (pause * 10), 10, led);
        addFade(0, 0, 0,                 0,                   25, 10, led);
      }
     





