#include <Adafruit_GFX.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>


// WIFI Credentials;
String wifiName = "";
String wifiPassword = "";

// Variables and Characters
int i = 0;
int delayedTime = 167;
bool isInName = true;
int differentLetterSystem = 0;
char upperCaseLetters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0'};
char lowerCaseLetters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0'};
char punctuations[] =     {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_','-', '+', '=', ';', ':', '<', '>', ',', '.', '?', '\''};
char digits[] =           {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
Preferences prfc;

// URL Of the api
String url = "https://api.open-meteo.com/v1/forecast?timezone=auto&latitude=23.7104&longitude=90.399452&current=temperature_2m,wind_speed_10m";

// GPIO Pins
#define gpio16 16
#define gpio17 17
#define gpio18 18
#define gpio19 19
#define gpio23 23

#define RESET_PIN -1
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define OLED_ADDRESS 0x3C

// Width comes first, then height
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, RESET_PIN);

void writeInMemory();
void draw_weatherUI();
void differentLetterSystemChanger();
void differentLetterSystemChanger2(String &obj);
void wifi_Interface(char i, String title = "Wi-Fi Name");

void setup() {
    Serial.begin(115200);
    prfc.begin("Wi-Fi_Cred", false);

    pinMode(gpio16, INPUT_PULLUP);
    pinMode(gpio17, INPUT_PULLUP);
    pinMode(gpio18, INPUT_PULLUP);
    pinMode(gpio19, INPUT_PULLUP);
    pinMode(gpio23, INPUT_PULLUP);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED initialization failed!");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(3, 10);
    wifi_Interface('A');
    display.display();
}

void loop() {
  if (prfc.getString("name") && prfc.getString("password")) {
      wifiName = prfc.getString("name");
      wifiPassword = prfc.getString("password");
      draw_weatherUI();
      return;
  }

  bool isLeft = (digitalRead(gpio16) == LOW);
  bool isRight = (digitalRead(gpio17) == LOW);
  bool isSysChgBtnPrsd = digitalRead(gpio18) == LOW;
  bool submitBtn = digitalRead(gpio19) == LOW;
  bool clearBtn = digitalRead(gpio23) == LOW;

  if (isRight && isLeft) {
    writeInMemory();
    delay(delayedTime + 50);
    return;
  }

  if (isLeft && i > 0) {
    i--;
    delay(delayedTime);
  }
  if (isRight && i < 25) {
    i++;
    delay(delayedTime);
  }

  if (isSysChgBtnPrsd) {
      differentLetterSystem++;

      if (differentLetterSystem > 3) 
        differentLetterSystem = 0;

      delay(delayedTime);
  }

  differentLetterSystemChanger();

  if (submitBtn && !clearBtn) {
    if (isInName) {
      differentLetterSystemChanger2(wifiName);
    }
    else {
      differentLetterSystemChanger2(wifiPassword);
    }
    delay(delayedTime);
  }
  
  if (clearBtn && !submitBtn && isInName) {
    wifiName.remove(wifiName.length()- 1);
    wifi_Interface(upperCaseLetters[i]);
    delay(delayedTime);
  }
  else if (clearBtn && !submitBtn && !isInName) {
    wifiPassword.remove(wifiPassword.length()- 1);
    wifi_Interface(upperCaseLetters[i], "Wi-Fi Password");
    delay(delayedTime);
  }

  if (submitBtn && clearBtn) {
    isInName = false;
    wifi_Interface(i, "Wi-Fi Password");
    delay(delayedTime + 50);
  }
}

void differentLetterSystemChanger() {
    display.clearDisplay();
    display.setCursor(3, 10);
    if (isInName) {
      if (differentLetterSystem == 0) wifi_Interface(upperCaseLetters[i]);
      else if (differentLetterSystem == 1) wifi_Interface(lowerCaseLetters[i]);
      else if (differentLetterSystem == 2) wifi_Interface(digits[i]);
      else wifi_Interface(punctuations[i]);
    }
    else {
      if (differentLetterSystem == 0) wifi_Interface(upperCaseLetters[i], "Wi-Fi Password");
      else if (differentLetterSystem == 1) wifi_Interface(lowerCaseLetters[i], "Wi-Fi Password");
      else if (differentLetterSystem == 2) wifi_Interface(digits[i], "Wi-Fi Password");
      else wifi_Interface(punctuations[i], "Wi-Fi Password");
    }
    display.display();
}

void differentLetterSystemChanger2(String &obj) {
  if (differentLetterSystem == 0) {
    obj += upperCaseLetters[i];
    delay(delayedTime);
    wifi_Interface(upperCaseLetters[i]);
  } 
  else if (differentLetterSystem == 1) {
    obj += lowerCaseLetters[i];
    delay(delayedTime);
    wifi_Interface(lowerCaseLetters[i]);
  }
  else if (differentLetterSystem == 2) {
    obj += digits[i];
    delay(delayedTime);
    wifi_Interface(digits[i]);
  }
  else {
    obj += punctuations[i];
    delay(delayedTime);
    wifi_Interface(punctuations[i]);
  }
}

void botton_SelectorBar(char i) {
  display.drawFastHLine(0, 44, SCREEN_WIDTH, SSD1306_WHITE);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 51);
  display.print(F("<  "));
  display.print(i);
  display.print(F("  >"));
}

void wifi_Interface(char i, String title) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Top header
  display.setCursor(0, 0);
  display.print(title);
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  // SSID display area
  display.setTextSize(1);
  display.setCursor(0, 16);
  if (title == "Wi-Fi Name") {
    display.println(wifiName);
    display.setTextWrap(true);
  }
  else {    
    display.println(wifiPassword);
    display.setTextWrap(true);
  }

  // Bottom selector bar
  botton_SelectorBar(i);

  display.display();
}

void writeInMemory() {
  prfc.putString("name", wifiName);
  prfc.putString("password", wifiPassword);

  Serial.println(prfc.getString("name"));
  Serial.println(prfc.getString("password"));
  
  prfc.end();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Saving Data....");
  display.display();
  delay(3000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 10);
  display.println("DONE");
  display.display();
  delay(1500);

  prfc.begin("Wi-Fi_Cred", true);
}

void draw_weatherUI() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(F("Connecting to WiFi..."));
  display.display();

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiName.c_str(), wifiPassword.c_str());
    Serial.println(wifiName);
    Serial.println(wifiPassword);
    unsigned long startMs = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startMs < 10000) {
      delay(250);
    }
  }

  display.clearDisplay();
  display.setCursor(0, 10);

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient http;
    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temp = doc["current"]["temperature_2m"];
        float windspeed = doc["current"]["wind_speed_10m"];

        display.print(F("Temp: "));
        display.print(temp);
        display.println(F(" C"));

        display.setCursor(0, 24);
        display.print(F("Wind: "));
        display.print(windspeed);
        display.println(F(" km/h"));
      } else {
        display.println(F("JSON Parse failed"));
      }
    } else {
      display.print(F("HTTP err: "));
      display.println(httpCode);
    }
    http.end();
  } else {
    display.println(F("WiFi failed"));
  }

  display.display();
  delay(10000);
}