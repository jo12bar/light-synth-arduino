#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define MSG_LENGTH 6

#define STRIP_LENGTH 30
#define LOW_STRIP_PIN 6
#define MID_STRIP_PIN 5
#define HIGH_STRIP_PIN 4

/**
 * dataBuf contains the full hex-encoded string of levels for the Neopixel strips,
 * where the first two bytes represent low, the next two are mid, and the final
 * two are high.
 */
char dataBuf[MSG_LENGTH + 1] = {0};
char lowBuf[MSG_LENGTH / 3 + 1] = {0};
char midBuf[MSG_LENGTH / 3 + 1] = {0};
char highBuf[MSG_LENGTH / 3 + 1] = {0};

byte low = 0;
byte mid = 0;
byte high = 0;

Adafruit_NeoPixel lowStrip = Adafruit_NeoPixel(STRIP_LENGTH, LOW_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel midStrip = Adafruit_NeoPixel(STRIP_LENGTH, MID_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel highStrip = Adafruit_NeoPixel(STRIP_LENGTH, HIGH_STRIP_PIN, NEO_GRB + NEO_KHZ800);

int lowColorChangeCounter = 0;
int midColorChangeCounter = 85;
int highColorChangeCounter = 170;

/** Change colour every n milliseconds */
const unsigned long colorChangeInterval = 30;
unsigned long prevColorChangeMillis = 0;

void setup() {
  Serial.begin(9600);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  lowStrip.begin();
  lowStrip.show();
  midStrip.begin();
  midStrip.show();
  highStrip.begin();
  highStrip.show();
}

void loop() {
  byte nLowPixels = map(low, 0, 255, 0, STRIP_LENGTH);
  byte nMidPixels = map(mid, 0, 255, 0, STRIP_LENGTH);
  byte nHighPixels = map(high, 0, 255, 0, STRIP_LENGTH);

  for (int i = 0; i < nLowPixels; i++) {
    lowStrip.setPixelColor(i, colorWheel(lowColorChangeCounter));
  }
  for (int i = 0; i < nMidPixels; i++) {
    midStrip.setPixelColor(i, colorWheel(midColorChangeCounter));
  }
  for (int i = 0; i < nHighPixels; i++) {
    highStrip.setPixelColor(i, colorWheel(highColorChangeCounter));
  }
  lowStrip.show();
  midStrip.show();
  highStrip.show();

  unsigned long currColorChangeMillis = millis();

  if ((unsigned long)(currColorChangeMillis - prevColorChangeMillis) >= colorChangeInterval) {
    lowColorChangeCounter++;
    midColorChangeCounter++;
    highColorChangeCounter++;
    prevColorChangeMillis = millis();
  }

  if (lowColorChangeCounter > 255) lowColorChangeCounter = 0;
  if (midColorChangeCounter > 255) midColorChangeCounter = 0;
  if (highColorChangeCounter > 255) highColorChangeCounter = 0;
}

/**
 * Called when data is available to read over Serial.
 * Reads the full string of bytes into dataBuf, and splits it up into lowBuf,
 * midBuf, and highBuf.
 */
void serialEvent() {
  // Zero out the memory.
  memset(dataBuf, 0, sizeof(dataBuf));
  memset(lowBuf, 0, sizeof(lowBuf));
  memset(midBuf, 0, sizeof(midBuf));
  memset(highBuf, 0, sizeof(highBuf));

  // Read & split data
  Serial.readBytes(dataBuf, MSG_LENGTH);
  strncpy(lowBuf, dataBuf, 2);
  strncpy(midBuf, dataBuf + 2, 2);
  strncpy(highBuf, dataBuf + 4, 2);

  // Terminate each buffer with a null character
  dataBuf[MSG_LENGTH] = '\0';
  lowBuf[MSG_LENGTH / 3] = '\0';
  midBuf[MSG_LENGTH / 3] = '\0';
  highBuf[MSG_LENGTH / 3] = '\0';

  // Convert component buffers into the byte values they represent.
  low = (byte)strtol(lowBuf, NULL, 16);
  mid = (byte)strtol(midBuf, NULL, 16);
  high = (byte)strtol(highBuf, NULL, 16);

  // clear all the pixels
  for (int i = 0; i < STRIP_LENGTH; i++) {
    lowStrip.setPixelColor(i, 0, 0, 0);
    midStrip.setPixelColor(i, 0, 0, 0);
    highStrip.setPixelColor(i, 0, 0, 0);
  }
  lowStrip.show();
  midStrip.show();
  highStrip.show();
}

/**
 * Input a value 0 to 255 to get a color value.
 * The colours are a transition r - g - b - back to r.
 */
uint32_t colorWheel(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return lowStrip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    return lowStrip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return lowStrip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

