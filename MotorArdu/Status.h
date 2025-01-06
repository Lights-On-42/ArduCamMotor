#include <Adafruit_NeoPixel.h>

#define LED_PIN     48  // GPIO48 auf deinem ESP32-S3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

void SetupStatusLED()
{
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initiale LED-Werte festlegen (aus)
}

void setLEDColor(uint8_t red, uint8_t green, uint8_t blue) 
{
  // Setze die Farbe für die erste LED
  strip.setPixelColor(0, strip.Color(red, green, blue));
  strip.show();  // Zeige die Farbe auf der LED
}

//Start oder Fehler
void SetAbort()
{
  setLEDColor(255, 0, 0); // Rot
}

//Stopped
void SetStopped()
{
  setLEDColor(128, 128, 0); // Gelb
}

//Init fertig
void SetIDLE()
{
  setLEDColor(0, 0, 255); // blau
  Serial.println("IDLE");
}

//Client Connectetd
void SetConnectet()
{
  setLEDColor(0, 255, 0); // Grün
}
