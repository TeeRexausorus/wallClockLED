#include <FastLED.h>

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define NUM_LEDS 18
#define DATA_PIN D6
const char* ssid = "TADDEI2";
const char* password = "sonicsonic";
CRGB leds[NUM_LEDS];
CRGB marron(255, 153, 100);
CRGB cyan(0, 245, 225);
CRGB white(255, 255, 255);
CRGB red(255, 0, 0);
WiFiClient espClient;
WiFiUDP ntpUDP;

uint8_t thishue = 0;
uint8_t deltahue = 15;
const int buttonPin = D1;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int mode = 1;
int pos = 0;
int pos2 = NUM_LEDS/2;
int tempo = 0;
int brightness = 5;
int nbModes = 6;            // nb Arbitraire pour le moment
bool wasDown = true;
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
  for (int i = 0; i < 20; ++i){
    analogVal += analogRead(A0);
  }
  analogVal /= 20;
  int val = map(analogVal, 450, 485, 10, 255);
  return val;
}

void loop() {
    // Met à jour l'heure toutes les 10 secondes - update time every 10 secondes
    timeClient.update();
    

    FastLED.setBrightness(getBrightnessFromPotar());
    buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) { // bouton ralâché
      wasDown = true;
      /*Serial.print(getHours(timeClient));
      Serial.print(":");
      Serial.println(timeClient.getMinutes());*/
    }
    else if (buttonState == HIGH && wasDown){
      mode = (mode + 1) % nbModes;
      
      wasDown = false;
    }

    switch(mode){
      case 0:
        FastLED.clear();
        FastLED.show(); 
        break;
      case 1:
        thishue--;
        fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
        FastLED.show();
        break;
      case 2:
        fill_solid(leds, 9, marron);
        fill_solid(leds+9, 9, cyan);
        FastLED.show(); 
        break;
      case 3:
        fill_solid(leds, NUM_LEDS, cyan);
        FastLED.show();
        break;
      case 4:
        fill_solid(leds, NUM_LEDS, white);
        leds[pos] = cyan;
        leds[pos2] = cyan;
        FastLED.show();
        if (tempo % 10 == 0){
          pos = (pos + 1) % NUM_LEDS;
          pos2 = (pos2 + 1) % NUM_LEDS;
        }
        tempo++;
        break;
      case 5:
        fill_solid(leds, NUM_LEDS, cyan);
        leds[getHours(timeClient) - 1] = red;
        FastLED.show();
        break;
      default:
        break;
    }
    
    int analogVal = analogRead(A0);
    int val = map(analogVal, 450, 485, 0, 255);
    Serial.println(val);
    delay(10);
}

int getHours(NTPClient timeClient){
  int hours = timeClient.getHours();
  hours = hours > 12 ? hours - 12 : hours;
  return hours;
}
