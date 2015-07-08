#include <FastLED.h>

#define NUM_LEDS 156
#define DATA_PIN 2
#define SERIALRATE 500000
#define CALIBRATION_TEMPERATURE TypicalLEDStrip  // Color correction
#define MAX_BRIGHTNESS 255 // 0-255

// Adalight sends a "Magic Word" (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;

CRGB leds[NUM_LEDS];

void setup()
{

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  FastLED.setTemperature( CALIBRATION_TEMPERATURE );
  FastLED.setBrightness( MAX_BRIGHTNESS );

  Serial.begin(SERIALRATE);
  Serial.print("Ada\n"); // Send "Magic Word" string to host

}

void loop() {
  // wait for first byte of Magic Word
  
  for(i = 0; i < sizeof prefix; ++i) {
    waitLoop: 
    while (!Serial.available());
    if (prefix[i] == Serial.read()) continue;
    i = 0;
    goto waitLoop;
  }

  // Hi, Lo, Checksum
  while (!Serial.available());
  hi = Serial.read();
  while (!Serial.available());
  lo = Serial.read();
  while (!Serial.available());
  chk = Serial.read();

  // if checksum does not match go back to wait
  if (chk != (hi ^ lo ^ 0x55)) {
    i = 0;
    goto waitLoop;
  }

  Serial.readBytes((char*)leds, NUM_LEDS*3);

  // shows new values
 FastLED.show();
}
