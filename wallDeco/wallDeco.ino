#include <FastLED.h>

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#define NUM_LEDS 72
#define  OFFSET_LEDS 14
#define DATA_PIN D6

WiFiServer server(80);

const char* ssid = "UnicornsAreReal";
const char* password = "Duck42You";

// Initialize the LED strip
CRGB leds[NUM_LEDS];

// We define the colors
CRGB marron(255, 153, 100);
CRGB cyan(0, 245, 225);
CRGB white(255, 255, 255);
CRGB red(255, 0, 0);
CRGB deeppink(0xFF1493);
WiFiClient espClient;
WiFiUDP ntpUDP;

uint8_t thishue = 0;
uint8_t deltahue = 15;

const int buttonPin = D1;           // the number of the pushbutton pin
int buttonState = LOW;              // variable for reading the pushbutton status

int mode = 2;                       // The mode selected
const int nbModes = 7;              // The number of available modes. Arbitrary for now.


int pos = 0;                        // The position for the LED trains
int pos2 = NUM_LEDS / 2;

int tempo = 0;                      // A small temporization feature

int brightness = 5;                 // The brightness of the LED strip

bool wasDown = true;                // A boolean to follow if this button was pressed or not.

int clockArray[] = {0, 67, 59, 54, 48, 40, 35, 28, 22, 17, 12, 5};     // An array for the clock

NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

int getBrightnessFromPotar(){
  int analogVal = 0;
  int avgDivider = 20;

  // Average the analog value
  for (int i = 0; i < avgDivider; ++i){
    analogVal += analogRead(A0);
  }
  analogVal /= avgDivider;
  
  return map(analogVal, 560, 660, 10, 255); // We map the value read by the potentiometer to the possible values of brightness
}

void setup() {
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("ExploratoireDeco");
  //wifiManager.resetSettings();

  // Start NTP client
  timeClient.begin();
  // Initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}


void loop() {
    // Serial.println(getBrightnessFromPotar());
    // Met à jour l'heure toutes les 10 secondes - update time every 10 secondes
    timeClient.update();
    
    /*
    int brightPotar = getBrightnessFromPotar();
    Serial.print(FastLED.getBrightness());
    Serial.print("     ");
    Serial.print(brightPotar);
    Serial.print("     ");
    Serial.println(abs(FastLED.getBrightness() - brightPotar));
    
    if (abs(FastLED.getBrightness() - brightPotar) > 5){
      // Serial.println(brightPotar);
      FastLED.setBrightness(brightPotar);
    }*/
    
    buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) {// Button presed
      wasDown = true;
    }
    else if (buttonState == HIGH && wasDown){// Button released
      mode = (mode + 1) % nbModes;
      
      wasDown = false;
    }

    switch(mode){
      case 0: // Software off
        clear();
        break;
      case 1: // Rainbow
        rainbow();
        break;
      case 2: // Half deeppink, half cyan
        halfHalf();
        break;
      case 3: // Full cyan
        full(cyan);
        break;
      case 4: // Cyan trains on white background
        trainColor(white, cyan);
        break;
      case 5: // Clock 
        clock(white, red);
        break;
      case 6: // pink trains on white background
        trainColor(white, deeppink);
        break;
      default:
        break;
    }
}

void clear(){
  FastLED.clear();
  FastLED.show();
}

void rainbow(){
  thishue--;
  fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
  FastLED.show();
}

void halfHalf(){
  fill_solid(leds, 6, cyan);
  fill_solid(leds + 6, (NUM_LEDS/2) + 6, deeppink);
  fill_solid(leds + (NUM_LEDS/2) + 6, (NUM_LEDS/2) - 6, cyan);
  FastLED.show();
}

void full(CRGB color){
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

void trainColor(CRGB bgColor, CRGB color){
  fill_solid(leds, NUM_LEDS, bgColor);
  leds[pos] = color;
  leds[pos2] = color;
  FastLED.show();
  if (tempo % 10 == 0){ //small software temporizing, as to not use delay and slow down the global execution
    pos = (pos + 1) % NUM_LEDS;
    pos2 = (pos2 + 1) % NUM_LEDS;
  }
  tempo++;
}

void clock(CRGB bgColor, CRGB hourColor){
        fill_solid(leds, NUM_LEDS, bgColor);
        leds[clockArray[getHours(timeClient) - 1]] = hourColor;
        FastLED.show();
        delay(90);
}

//Function that returns the hour in a 0-12 fashion
int getHours(NTPClient timeClient){
  int hours = timeClient.getHours();
  hours = hours > 12 ? hours - 12 : hours;
  return hours;
}
