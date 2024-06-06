#include "AGS02MA.h"
#include "GazoTimeHelpers.h"
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include <Arduino.h>
#include "DYPlayerArduino.h"

#define CONSOLE_IP "192.168.0.36"
#define CONSOLE_PORT 8273

AGS02MA AGS(26);
LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiUDP Udp;
DY::Player player;

unsigned int ss=0; // seconds
unsigned long lastTick=millis(); // time that the clock last "ticked"

const char* ssid = "Tenda_AX3000_2G";
const char* password = "TarakanovnaMojaTarakanovna";
unsigned long previousMillis = 0;
unsigned long interval = 30000;
int version, resist;
bool alarmTrigger = false;

const int relayPin = 33;
uint32_t indexMeasarray = 0;
uint32_t arrayLength = 5000;
uint32_t arrayOfMeasurements[5000];

void setup()
{
  delay(1000);

  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.println();

  Wire.begin();

  connecToWiFi();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  lcd.init();
	lcd.backlight();
	lcd.print("Start!");

  initAGS02MA();
  initPlayer();
}

void initAGS02MA() {
  Serial.print("AGS02MA_LIB_VERSION: ");
  Serial.println(AGS02MA_LIB_VERSION);
  Serial.println();

  bool b = AGS.begin();
  Serial.print("BEGIN:\t");
  Serial.println(b);

  uint32_t dd = AGS.getSensorDate();
  Serial.print("Sensor date:\t");
  Serial.println(dd, HEX);   //  prints YYYYMMDD e.g. 20210203

  b = AGS.setPPBMode();
  uint32_t m = AGS.getMode();
  Serial.print("MODE:\t");
  Serial.print(b);
  Serial.print("\t");
  Serial.println(m);

  uint8_t version = AGS.getSensorVersion();
  Serial.print("VERS:\t");
  Serial.println(version);
}

void loop()
{
  Serial.println("-------->");
  delay(1000);
  uint32_t ppbValue = AGS.readPPB();
  char * uptime = uptimes();

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("PPB: ");
  lcd.print(ppbValue);
  lcd.setCursor(0,3);
  lcd.print(uptime); 
  Serial.print("PPB: ");
  Serial.print(ppbValue);
  Serial.print(", Uptime: ");
  Serial.println(uptime);

  checkWiFi();

  sendUdp(buildString("PPB: ", ppbValue));
  if (checkOverload(ppbValue)) {
    if (!alarmTrigger) {
      playAlarm();
      alarmTrigger = true;
    }
    Serial.println("-------->");
    Serial.println("LOW");
    digitalWrite(relayPin, LOW);
    Serial.println("-------->");
  } else {
    alarmTrigger = false;
    Serial.println("-------->");
    Serial.println("HIGH");
    digitalWrite(relayPin, HIGH);
    Serial.println("-------->");
  }
}

uint8_t countDigits(int num)
{
   return((num<0?2:1)+(int8_t)log10(abs(num))) ;
}

void connecToWiFi() {
  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  uint8_t counter = 1;
  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
      if (counter > 100) {
        counter = 0;
        WiFi.reconnect();
      }
      counter++;
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void initPlayer() {
  Serial.flush();
  delay(1000);
  Serial.begin(9600);
  player.begin(); 
  player.setVolume(30);
  Serial.flush();
  delay(1000);
  Serial.begin(115200);
}

void playAlarm() {
  Serial.flush();
  delay(1000);
  Serial.begin(9600);
  player.playSpecified(1);
  Serial.flush();
  delay(10000);
  Serial.begin(115200);
}

char * uptimes() {
  unsigned long millisPassed = millis() - lastTick;
  if (millisPassed >= 1000UL) {
    lastTick += millisPassed;
    ss=(lastTick/1000UL);
    return seconds2duration(ss, true);
  }
  return "-";
}

bool checkOverload(uint32_t ppbValue) {
  if (indexMeasarray < arrayLength) {
      Serial.println("-----> Накопление данных.");
      Serial.print("Append index: ");
      Serial.println(indexMeasarray);
      Serial.print("Append value: ");
      Serial.println(ppbValue);
      lcd.setCursor(0,1);
      lcd.print("Nakoplenie dannyh: "); 
      lcd.setCursor(0,2);
      lcd.print(indexMeasarray);
      lcd.print(" iz ");
      lcd.print(arrayLength);
      if (ppbValue > 0) {
        arrayOfMeasurements[indexMeasarray] = {ppbValue};
        indexMeasarray++;
      }
      return false;
  } else {
      uint32_t i = 0;
      uint32_t fullData = 0;
      while (i < indexMeasarray) {
        arrayOfMeasurements[i] = arrayOfMeasurements[i+1];
        i++;
        fullData += arrayOfMeasurements[i];
      }
      uint32_t averageСomplete = fullData/indexMeasarray;
      arrayOfMeasurements[indexMeasarray-1] = {ppbValue};
      Serial.print("Overload append index: ");
      Serial.println(indexMeasarray);
      Serial.print("Overload array : ");
      uint32_t razmer = sizeof(arrayOfMeasurements) / sizeof(arrayOfMeasurements[0]);
      Serial.println(razmer);
      Serial.print("Overload, array 0: ");
      Serial.println(arrayOfMeasurements[0]);
      Serial.print("Average full: ");
      Serial.println(averageСomplete);
      uint32_t recentData = 0;
      uint32_t recentValues = indexMeasarray - 20;
      while (recentValues < indexMeasarray) {
        recentData += arrayOfMeasurements[recentValues];
        recentValues++;
      }
      uint32_t averageRecent = recentData/20;
      Serial.print("Recent 20 average: ");
      Serial.println(averageRecent);

      float percentageDifference = ((float)averageRecent/(float)averageСomplete) * 100 - 100;
      Serial.print("Превышение: ");
      Serial.print(percentageDifference);
      Serial.println("%");

      lcd.setCursor(0,1);
      lcd.print("Prevyshenie: "); 
      lcd.print(percentageDifference);
      lcd.print("%"); 

      sendUdp(buildString("Prevyshenie: ", percentageDifference, "%%"));

      return percentageDifference > 30 || (percentageDifference > 10 && alarmTrigger);
  }
}

void checkWiFi() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

String buildString(char* literal, uint32_t number) {
    uint8_t numberSize = countDigits(number);
    uint8_t literalSize = strlen(literal);
    char buf[numberSize+literalSize];
    char buffer [numberSize];
    char* append = itoa(number, buffer, 10);
    strcpy(buf, literal);
    strcpy(buf + literalSize, append);
    String out = buf;
    return out;
}

String buildString(char* literal, float number, char* suffix) {
    uint8_t literalSize = strlen(literal);
    uint8_t numberSize = countDigits(number);
    uint8_t suffixSize = strlen(suffix);
    char buf[numberSize+literalSize+suffixSize];
    char buffer [numberSize];
    char* append = itoa(number, buffer, 10);
    strcpy(buf, literal);
    strcpy(buf + literalSize, append);
    strcpy(buf + literalSize + numberSize, suffix);
    String out = buf;
    return out;
}

void sendUdp(String str) {
  Udp.beginPacket(CONSOLE_IP, CONSOLE_PORT);
  Serial.print("Send udp: ");
  Serial.println(str.c_str());
  Udp.printf(str.c_str(),str.length());
  Udp.endPacket();
}
