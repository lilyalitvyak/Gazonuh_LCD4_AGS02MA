#include <Arduino.h>
#include "BuildString.h"

void connecToWiFi();
void initAGS02MA();
void initPlayer();
char * uptimesRu();
void checkWiFi();
void connecToWiFi();
void sendUdp(String str);
bool checkOverload(uint32_t ppbValue);
void playAlarm();
void webServer(uint32_t ppbValue, char * uptime);
