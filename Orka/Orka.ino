#include <Wire.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WIFI Credentials;
String wifiName;
String wifiPassword;

// Variables and Characters
int i = 0;
bool isInName = true;
int differentLetterSystem = 0;
char upperCaseLetters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0'};
char lowerCaseLetters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0'};
char punctuations[] =     {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_','-', '+', '=', ';', ':', '<', '>', ',', '.', '?', '\''};
char digits[] =           {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
Preferences preference;

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

void differentLetterSystemChanger();
void differentLetterSystemChanger2(String &obj);
void wifi_Interface(char i, String title = "Wi-Fi Name");

void setup() {
    Serial.begin(115200);
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
  bool isLeft = (digitalRead(gpio16) == LOW);
  bool isRight = (digitalRead(gpio17) == LOW);
  bool isSysChgBtnPrsd = digitalRead(gpio18) == LOW;
  bool submitBtn = digitalRead(gpio19) == LOW;
  bool clearBtn = digitalRead(gpio23) == LOW;

  if (isLeft && i > 0)
  {
    i--;
    delay(167);
  }
  if (isRight && i < 25)
  {
    i++;
    delay(167);
  }

  if (isSysChgBtnPrsd)
  {
      differentLetterSystem++;

      if (differentLetterSystem > 3)
          differentLetterSystem = 0;

      delay(167);
  }

  differentLetterSystemChanger();

  if (submitBtn && !clearBtn)
  {
    if (isInName)
    {
      differentLetterSystemChanger2(wifiName);
    }
    else
    {
      differentLetterSystemChanger2(wifiPassword);
    }
    delay(167);
  }
  
  if (clearBtn && !submitBtn)
  {
    wifiName.remove(wifiName.length()- 1);
    wifi_Interface(upperCaseLetters[i]);
    delay(167);
  }

  if (submitBtn && clearBtn)
  {
    wifi_Interface(i, "Wi-Fi Password");
  }
}

void differentLetterSystemChanger()
{
    display.clearDisplay();
    display.setCursor(3, 10);
    if (differentLetterSystem == 0) wifi_Interface(upperCaseLetters[i]);
    else if (differentLetterSystem == 1) wifi_Interface(lowerCaseLetters[i]);
    else if (differentLetterSystem == 2) wifi_Interface(digits[i]);
    else wifi_Interface(punctuations[i]);
    display.display();
}

void differentLetterSystemChanger2(String &obj)
{
  if (differentLetterSystem == 0)
  {
    obj += upperCaseLetters[i];
    delay(167);
    wifi_Interface(upperCaseLetters[i]);
  } 
  else if (differentLetterSystem == 1) {
    obj += lowerCaseLetters[i];
    delay(167);
    wifi_Interface(lowerCaseLetters[i]);
  }
  else if (differentLetterSystem == 2) {
    obj += digits[i];
    delay(167);
    wifi_Interface(digits[i]);
  }
  else {
    obj += punctuations[i];
    delay(167);
    wifi_Interface(punctuations[i]);
  }
}

void botton_SelectorBar(char i) {
  display.drawFastHLine(0, 44, SCREEN_WIDTH, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 51);
  display.print(F("<    "));
  display.print(i);
  display.print(F("    >"));
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
  display.setTextSize(2);
  display.setCursor(0, 16);
  if (title == "Wi-Fi Name")
  {
    display.println(wifiName);
  }
  else
  {    
    display.println(wifiPassword);
  }

  // Bottom selector bar
  botton_SelectorBar(i);

  display.display();
}