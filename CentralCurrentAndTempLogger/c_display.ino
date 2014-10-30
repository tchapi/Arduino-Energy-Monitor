// LED strip is connected to pin 6
#define LED_STRIP_PIN 6

// Neopixels libs
#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Connect CLK to the I2C clock - on Arduino UNO thats Analog #5, on the Leonardo it's Digital #3, on the Mega it's digital #21
// Connect DAT to the I2C data - on Arduino UNO thats Analog #4, on the Leonardo it's Digital #2, on the Mega it's digital #20
// Connect GND to common ground
// Connect VCC+ to power - 5V
Adafruit_7segment matrix = Adafruit_7segment();

// Create 6 colors for later use (http://www.bretttolbert.com/projects/colorscale/)
const uint32_t blue = strip.Color(2, 204, 253);
const uint32_t turquoise = strip.Color(2, 253, 141);
const uint32_t green = strip.Color(22, 254, 1);
const uint32_t yellow = strip.Color(184, 254, 1);
const uint32_t orange = strip.Color(255, 163, 0);
const uint32_t red = strip.Color(255, 0, 0);
const uint32_t white = strip.Color(255, 255, 255);
const uint32_t colors[] = {
  white, white, blue, turquoise, green, yellow, orange, red};

// Number of leds to lit from the 6 of the strip for W
int nb_leds = 0;

// We need this to not display bogus values on first loop
boolean first_loop = true;
