#include <OpenWeather.h>
#include <JSON_Decoder.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <WiFi.h>
#include <HTTPClient.h>

//pin de l'Arduino de la matriu de leds
#define PIN            25
// Numero de leds connectats
#define NUMPIXELS      64

//----------WIFI----------
const char* ssid = ""; //------------------------------------------TYPE WIFI SSID-------------------------
const char* password = ""; //--------------------------------------TYPE WIFI PASSWORD---------------------
bool wifiStatus = false;

//-------OPEN WEATHER-----
String api_key = ""; //---------------------TYPE API KEY--------------------------
String latitude =  ""; //-------------------TYPE UBI Latitude--------------------------
String longitude = ""; //-------------------TYPE UBI Longitude--------------------------
String units = "metric"; 
String language = "en"; 
OW_Weather ow;

//----TIMING VARIABLES-----
const unsigned long screenInterval = 5000; //5s between pages
unsigned long previousScreenTime = 0;

const unsigned long weatherUpdateInterval = 300000; //5min between weather updates
unsigned long previousWeatherUpdateTime = 0;

//----SCREEN PAGE STATE----
int currentScreen = 0;
int totalScreens = 3;

//----LOCAL WEATHER VAR----
int temp = 0; 
int hum = 0;
char condition = 0;

//----0-9 PIXEL ART NUMS----
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

//----PIXEL ART CONDITIONS----
//SUN (01d)
int sunPos[] = {0,7,9,14,20,19,29,28,27,26,37,36,35,34,44,43,54,49,63,56};
int sunR[] = {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25};
int sunG[] = {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25};
int sunB[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//CLOUD(03d,04d)
int cloudPos[]={39, 30, 29, 28, 27, 26, 25, 38, 37, 36, 35, 34, 33, 32, 47, 46, 45, 44, 43, 42, 54, 53, 52, 51, 61, 60, 59, 50, 41, 40};
int cloudR[] = {15, 15, 15, 15, 15, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25};
int cloudG[] = {15, 15, 15, 15, 15, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25};
int cloudB[] = {10, 10, 10, 10, 10, 10, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 35, 35, 35, 35, 35};

//Rain Aditive (09d,010d)
int rainADDPos[] = {8, 10, 12, 14, 17, 19, 21, 23};
int rainADDR[] = {0, 0, 0, 0, 0, 0, 0, 0};
int rainADDG[] = {0, 0, 0, 0, 0, 0, 0, 0};
int rainADDB[] = {25, 25, 25, 25, 25, 25, 25, 25};

//thunder Aditive (11d)
int thunderADDPos[] = {2, 8, 10, 11, 12, 14, 17, 19, 21, 23};
int thunderADDR[] = {25, 0, 25, 0, 0, 0, 0, 25, 0, 0};
int thunderADDG[] = {25, 0, 25, 0, 0, 0, 0, 25, 0, 0};
int thunderADDB[] = {0, 25, 0, 0, 25, 25, 25, 0, 25, 25};

//Sun Additive (02d)
int sunADDPos[]={33, 41, 42, 50, 51};
int sunADDR[] = {25, 25, 25, 25, 25};
int sunADDG[] = {25, 25, 25, 25, 25};
int sunADDB[] = {0, 0, 0, 0, 0};

int* numbers[] = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {  
  Serial.begin(115200); //Initialize serial
  pixels.begin(); // Initialize Pixels
  DisplayNoData(); //Display no data (- -)
  pixels.show();
  ConnectWifi(); //Wifi conection
}

void loop() {
  unsigned long currentMilis = millis();

  //SCREEN ROTATION
  if(currentMilis - previousScreenTime >= screenInterval){
    previousScreenTime = currentMilis;
    currentScreen = (currentScreen+1) % totalScreens;
    displayScreen(currentScreen);
  }

  //WEATHER UPDATE
  if (currentMilis - previousWeatherUpdateTime >= weatherUpdateInterval){
    previousWeatherUpdateTime = currentMilis;
    temp = round(getWeather(1));
    temp = round(temp);

    hum = round(getWeather(2));
    hum = round(hum);

    condition = getWeather(0);
  }

  wifiStatusPixel();
  while (temp == 0){
    temp = round(getWeather(1));
    temp = round(temp);

    hum = round(getWeather(2));
    hum = round(hum);
    Serial.print(hum);

    condition = getWeather(0);

    DisplayNoData();
    delay(2000);
  }
 
  pixels.show();
}

void DisplayCondition(){
  if(condition == 0){
    DisplayNoData();
  }else if(condition == 1){
    for (int i = 0; i < sizeof(sunPos) / sizeof(sunPos[0]); i++) {
      pixels.setPixelColor(sunPos[i], sunR[i], sunG[i], sunB[i]);
    }
  }else if(condition == 2){
    for (int i = 0; i < sizeof(cloudPos) / sizeof(cloudPos[0]); i++) {
      pixels.setPixelColor(cloudPos[i]-16, cloudR[i], cloudG[i], cloudB[i]);
    }
    for (int i = 0; i < sizeof(sunADDPos) / sizeof(sunADDPos[0]); i++) {
      pixels.setPixelColor(sunADDPos[i], sunADDR[i], sunADDG[i], sunADDB[i]);
    }
  }else if(condition == 3){
    for (int i = 0; i < sizeof(cloudPos) / sizeof(cloudPos[0]); i++) {
      pixels.setPixelColor(cloudPos[i], cloudR[i], cloudG[i], cloudB[i]);
    }
  }else if(condition == 4){
    for (int i = 0; i < sizeof(cloudPos) / sizeof(cloudPos[0]); i++) {
      pixels.setPixelColor(cloudPos[i], cloudR[i], cloudG[i], cloudB[i]);
    }
    for (int i = 0; i < sizeof(rainADDPos) / sizeof(rainADDPos[0]); i++) {
      pixels.setPixelColor(rainADDPos[i], rainADDR[i], rainADDG[i], rainADDB[i]);
    }
  }else if(condition == 5){
    for (int i = 0; i < sizeof(cloudPos) / sizeof(cloudPos[0]); i++) {
      pixels.setPixelColor(cloudPos[i], cloudR[i], cloudG[i], cloudB[i]);
    }
    for (int i = 0; i < sizeof(thunderADDPos) / sizeof(thunderADDPos[0]); i++) {
      pixels.setPixelColor(thunderADDPos[i], thunderADDR[i], thunderADDG[i], thunderADDB[i]);
    }
  }
}

void DisplayHumidity(){
  if(hum == 0){
    DisplayNoData();
  }else{
    DisplayNumber(hum, 15);
  }
}

void displayScreen(int screen) {
  ClearDisplay();
  // Display the content based on the current screen
  switch (screen) {
    case 0:
      // Display Conditions
      DisplayCondition();
      pixels.setPixelColor(7, 0, 5, 50);
      pixels.setPixelColor(6, 0, 5, 50);
      pixels.setPixelColor(5, 0, 5, 50);
      pixels.setPixelColor(4, 0, 5, 50);
      pixels.setPixelColor(3, 5, 5, 0);
      pixels.setPixelColor(2, 5, 5, 0);
      pixels.setPixelColor(1, 5, 0, 5);
      pixels.setPixelColor(0, 5, 0, 5);
      break;
    case 1:
      // Display temperature
      DisplayNumber(temp, 15);
      pixels.setPixelColor(7, 0, 0, 1);
      pixels.setPixelColor(6, 0, 0, 1);
      pixels.setPixelColor(5, 0, 0, 1);
      pixels.setPixelColor(4, 0, 0, 1);
      pixels.setPixelColor(3, 30, 30, 0);
      pixels.setPixelColor(2, 30, 30, 0);
      pixels.setPixelColor(1, 1, 0, 1);
      pixels.setPixelColor(0, 1, 0, 1);
      break;
    case 2:
      // Display humidity
      DisplayHumidity();
      pixels.setPixelColor(7, 0, 0, 1);
      pixels.setPixelColor(6, 0, 0, 1);
      pixels.setPixelColor(5, 0, 0, 1);
      pixels.setPixelColor(4, 0, 0, 1);
      pixels.setPixelColor(3, 1, 1, 0);
      pixels.setPixelColor(2, 1, 1, 0);
      pixels.setPixelColor(1, 30, 0, 30);
      pixels.setPixelColor(0, 30, 0, 30);
      break;
  }
}

float getWeather(int dataType) {
  pixels.setPixelColor(57, 25, 25, 0);
  OW_forecast *forecast = new OW_forecast;

  Serial.print("\nRequesting weather information from OpenWeather... ");

  bool success = ow.getForecast(forecast, api_key, latitude, longitude, units, language);
  pixels.setPixelColor(57, 0, 0, 0);

  float result = 0; // Default result in case of failure

  if (success) {
    if (dataType == 0) {
      String icon = forecast->icon[0];
      Serial.print(icon);

      if (icon == "01d") {
        result = 1;
      } else if (icon == "02d") {
        result = 2;
      } else if (icon == "03d" || icon == "04d") {
        result = 3;
      } else if (icon == "09d" || icon == "10d") {
        result = 4;
      } else if (icon == "11d") {
        result = 5;
      } else {
        result = 0;
      }
    } else if (dataType == 1) {
      result = forecast->temp[0];
    } else if (dataType == 2) {
      result = forecast->humidity[0];
    }
  }

  delete forecast;
  return result;
}

void wifiStatusPixel(){
  if (WiFi.status() == WL_CONNECTED) {
    pixels.setPixelColor(56, 0,20,0);
  } else {
    pixels.setPixelColor(56, 20,0,0);
  }
  pixels.show();
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
  pixels.setPixelColor(56, 20,0,0);
  pixels.show();
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
}

void ClearDisplay(){
  for(int i = 0; i < 64; i++){ 
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
