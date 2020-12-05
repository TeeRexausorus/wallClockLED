#include <FastLED.h>

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define NUM_LEDS 18
#define DATA_PIN D6


const char* ssid = "*******";
const char* password = "********";

CRGB leds[NUM_LEDS];

// We define the colors
CRGB marron(255, 153, 100);
CRGB cyan(0, 245, 225);
CRGB white(255, 255, 255);
CRGB red(255, 0, 0);

WiFiClient espClient;
WiFiUDP ntpUDP;

uint8_t thishue = 0;
uint8_t deltahue = 15;

const int buttonPin = D1;           // the number of the pushbutton pin
int buttonState = LOW;              // variable for reading the pushbutton status

int mode = 6;                       // The mode selected
const int nbModes = 6;              // The number of available modes. Arbitrary for now.


int pos = 0;                        // The position for the LED trains
int pos2 = NUM_LEDS / 2;

int tempo = 0;                      // A small temporization feature

int brightness = 5;                 // The brightness of the LED strip

bool wasDown = true;                // A boolean to follow if this button was pressed or not.

NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  Serial.begin(115200);
  Serial.println("");
  Serial.print("Startup reason:");Serial.println(ESP.getResetReason());

  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi.");
  int _try = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("..");
    delay(500);
  }
  Serial.println("Connected to the WiFi network");
  
  // Démarrage du client NTP - Start NTP client
  timeClient.begin();
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

int getBrightnessFromPotar(){
  int analogVal = 0;
  int avgDivider = 20;

  // Average the analog value
  for (int i = 0; i < avgDivider; ++i){
    analogVal += analogRead(A0);
  }
  analogVal /= avgDivider;



  return map(analogVal, 450, 485, 10, 255); // We map the value read by the potentiometer to the possible values of brightness
}

void loop() {
    // Met à jour l'heure toutes les 10 secondes - update time every 10 secondes
    timeClient.update();
    

    FastLED.setBrightness(getBrightnessFromPotar());
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
        FastLED.clear();
        FastLED.show(); 
        break;
      case 1: // Rainbow
        thishue--;
        fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
        FastLED.show();
        break;
      case 2: // Half "brown", half cyan
        fill_solid(leds, 9, marron);
        fill_solid(leds+9, 9, cyan);
        FastLED.show(); 
        break;
      case 3: // Full cyan
        fill_solid(leds, NUM_LEDS, cyan);
        FastLED.show();
        break;
      case 4: // Cyan trains on white background
        fill_solid(leds, NUM_LEDS, white);
        leds[pos] = cyan;
        leds[pos2] = cyan;
        FastLED.show();
        if (tempo % 10 == 0){ //small software temporizing, as to not use delay and slow down the global execution
          pos = (pos + 1) % NUM_LEDS;
          pos2 = (pos2 + 1) % NUM_LEDS;
        }
        tempo++;
        break;
      case 5: // Clock
        fill_solid(leds, NUM_LEDS, cyan);
        leds[getHours(timeClient) - 1] = red;
        FastLED.show();
        break;
      default:
        break;
    }

    delay(10);
}

//Function that returns the hour in a 0-12 fashion
int getHours(NTPClient timeClient){
  int hours = timeClient.getHours();
  hours = hours > 12 ? hours - 12 : hours;
  return hours;
}
