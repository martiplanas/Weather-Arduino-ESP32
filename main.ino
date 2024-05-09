#include <OpenWeather.h>
#include <JSON_Decoder.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <WiFi.h>
#include <HTTPClient.h>

// Definim el pin de l'Arduino que utilitzarem per controlar el led o la matriu de leds
#define PIN            25
// Nยบ de leds connectats
#define NUMPIXELS      64

const char* ssid = "test";
const char* password = "arduinop";

String api_key = "9a77e9742b30c25a5d4745718be35c17"; 

String latitude =  "42.9831"; 
String longitude = "2.8249";

String units = "metric"; 
String language = "en"; 

OW_Weather ow;
int temp = 0; 
int hum

// Pixel art numbers
int num0[] = {12, 13, 14, 22, 30, 38, 46, 54, 53, 52, 44, 36, 28, 20};
int num1[] = {14, 13, 12, 21, 29, 37, 45, 53, 54};
int num2[] = {12, 13, 14, 22, 30, 29, 28, 36, 44, 52, 53, 54};
int num3[] = {12, 13, 14, 20, 28, 29, 36, 44, 52, 53, 54};
int num4[] = {12, 20, 28, 29, 30, 38, 36, 46, 44, 54, 52};
int num5[] = {12, 13, 14, 20, 28, 29, 30, 38, 46, 52, 53, 54};
int num6[] = {12, 13, 14, 20, 28, 29, 30, 38, 46, 54, 53, 52, 22};
int num7[] = {12, 20, 29, 28, 36, 44, 52, 53, 54};
int num8[] = {12, 20, 29, 28, 36, 44, 52, 53, 54, 13, 14, 22, 30, 38, 46};
int num9[] = {12, 20, 28, 29, 30, 38, 36, 46, 44, 52, 53, 54};

//Pixel art weather condition
//Sun
int sunPos[] = {0,7,9,14,20,19,29,28,27,26,37,36,35,34,44,43,54,49,63,56}
int sunR[] = {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25}
int sunG[] = {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25}
int sunB[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
//Cloud
int cloudPos[]={39, 30, 29, 28, 27, 26, 25, 38, 37, 36, 35, 34, 33, 32, 47, 46, 45, 44, 43, 43, 54, 53, 52, 51, 61, 60, 59, 50, 41, 40}
int cloudR[] = {15, 15, 15, 15, 15, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25}
int cloudG[] = {15, 15, 15, 15, 15, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25}
int cloudB[] = {15, 15, 15, 15, 15, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25}

//Rain Aditive
int rainADDPos[] = {0, 2, 4, 6, 17, 19, 21, 23}
int rainADDR[] = {0, 0, 0, 0, 0, 0, 0, 0}
int rainADDG[] = {0, 0, 0, 0, 0, 0, 0, 0}
int rainADDB[] = {25, 25, 25, 25, 25, 25, 25, 25}

//thunder Aditive
int rainADDPos[] = {0, 2, 6, 17, 19, 23, 21, 13, 12, 4}
int rainADDR[] = {0, 0, 0, 0, 0, 0, 25, 25, 25, 25}
int rainADDG[] = {0, 0, 0, 0, 0, 0, 25, 25, 25, 25}
int rainADDB[] = {25, 25, 25, 25, 25, 25, 0, 0, 0, 0}

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {  
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  ConnectWifi();
  DisplayNoData();
  pixels.show();
}

void loop() {
  while (temp == 0){
    temp = round(getWeather());
    temp = round(temp);
    DisplayNoData();
    delay(5000);
  }
  ClearDisplay();
  DisplayNumber(temp, 25);

  pixels.show();
  delay(5 * 60 * 1000);
}

float getWeather()
{
  OW_forecast  *forecast = new OW_forecast;

  Serial.print("\nRequesting weather information from OpenWeather... ");

  ow.getForecast(forecast, api_key, latitude, longitude, units, language);
  return forecast->temp[0];
  delete forecast;
}


void DisplayNumber(int num, int intensity){//ONLY 0 to 99
  int d1;
  int d2;

  //Separar numero per digits
  if(num < 10) {
    d1 = 0; 
    d2 = num; 
  } else {
    d1 = num / 10; 
    d2 = num % 10; 

    //Serial.print(d1);
    //Serial.print(d2);
  }


  for(int i = 0; i < 2; i++){
    int n;
    int m;

    //prepares variables depending on loop and number position
    if(i == 0){
      n = d1;
      m = 0;
    }else if (i == 1){
      n = d2;
      m = -4;
    }
    //Serial.print(n);
    //Draw numbers
    if (n == 0){
      for (int i = 0; i < sizeof(num0) / sizeof(num0[0]); i++) {
        int numberOfLef = num0[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 1){
      for (int i = 0; i < sizeof(num1) / sizeof(num1[0]); i++) {
        int numberOfLef = num1[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 2){
      for (int i = 0; i < sizeof(num2) / sizeof(num2[0]); i++) {
        int numberOfLef = num2[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 3){
      for (int i = 0; i < sizeof(num3) / sizeof(num3[0]); i++) {
        int numberOfLef = num3[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 4){
      for (int i = 0; i < sizeof(num4) / sizeof(num4[0]); i++) {
        int numberOfLef = num4[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 5){
      for (int i = 0; i < sizeof(num5) / sizeof(num5[0]); i++) {
        int numberOfLef = num5[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 6){
      for (int i = 0; i < sizeof(num6) / sizeof(num6[0]); i++) {
        int numberOfLef = num6[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 7){
      for (int i = 0; i < sizeof(num7) / sizeof(num7[0]); i++) {
        int numberOfLef = num7[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 8){
      for (int i = 0; i < sizeof(num8) / sizeof(num8[0]); i++) {
        int numberOfLef = num8[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
    if (n == 9){
      for (int i = 0; i < sizeof(num9) / sizeof(num9[0]); i++) {
        int numberOfLef = num9[i];
        pixels.setPixelColor(numberOfLef+m, intensity, intensity, intensity);
      }
    }
  }

}

void ConnectWifi(){
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
}

void ClearDisplay(){
  for(int i = 0; i < 63; i++){ 
    pixels.setPixelColor(i, 0, 0, 0);
  }
}

void DisplayNoData(){ //Display no-data indicator
  for(int i = 0; i < 63; i++){ 
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.setPixelColor(25, 25, 25, 25);
  pixels.setPixelColor(26, 25, 25, 25);
  pixels.setPixelColor(27, 25, 25, 25);

  pixels.setPixelColor(29, 25, 25, 25);
  pixels.setPixelColor(30, 25, 25, 25);
  pixels.setPixelColor(31, 25, 25, 25);
}