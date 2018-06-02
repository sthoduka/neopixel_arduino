#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define LB 10 // LED brightness
char current_command = '0';
char current_colour = 'w';
uint16_t arg = 0;


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  Serial.begin(9600);

  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (Serial.available() > 0 || current_command != '0')
  {
      char c;
      bool old_command = false;
      if (Serial.available() > 0)
      {
        c = Serial.read();
      }
      else
      {
        c = current_command;
        old_command = true;
      }
      
      if (c == 'b')// blink (example: b2r will blink red twice)
      {
        int how_many = Serial.parseInt();
        char colour = Serial.read();
        blinkRing(how_many, getColour(colour));
        current_command = '0';
      }
      else if (c == 'c') // cycle (example: c2r will cycle red twice)
      {
        int how_many = Serial.parseInt();
        char colour = Serial.read();
        colourWipe(how_many, getColour(colour));
        current_command = '0';
      }
      else if (c == 's') // spin (example: sg will spin green twice)
      {
        char colour;
        if (old_command)
        {
          colour = current_colour;
          arg = (arg + 1) % pixels.numPixels();
        }
        else
        {
          current_command = c;
          arg = 1;
          colour = Serial.read();
          Serial.print(colour);
          current_colour = colour;
        }
        setFirstPixels(arg, getColour(colour));
      }
  }
}
uint32_t getColour(char c)
{
  if (c == 'r') return pixels.Color(LB, 0, 0);
  if (c == 'g') return pixels.Color(0, LB, 0);
  if (c == 'b') return pixels.Color(0, 0, LB);
  return pixels.Color(LB, LB, LB);
}

void blinkRing(uint32_t how_many, uint32_t colour)
{
  for (uint16_t j = 0; j < how_many; j++)
  {
    for (uint16_t i = 0; i < pixels.numPixels(); i++)
    {
      pixels.setPixelColor(i, colour);
    }
    pixels.show();
    delay(250);
    pixels.clear();
    pixels.show();
    delay(250);
  }
}

// Fill the dots one after the other with a color
void colourWipe(uint32_t how_many, uint32_t colour) {
  for(uint16_t j = 0; j < how_many; j++)
  {
    for(uint16_t i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, colour);
      pixels.show();
      delay(50);
    }
    pixels.clear();
    pixels.show();
  }
}

void setFirstPixels(uint32_t index, uint32_t colour)
{
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    if (i != index)
    {
      pixels.setPixelColor(i, colour);
    }
    else
    {
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
  }
  pixels.show();
  delay(index * 15);
}
