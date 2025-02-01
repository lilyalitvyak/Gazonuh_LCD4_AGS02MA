#ifndef _GNWEBSERVER_H_
#define _GNWEBSERVER_H_ 

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

class GNWebServer
{
private:
    const char* ssid;  
    const char* password; 
    const WebServer server;

public:
    GNWebServer();
    void update(uint32_t ppbValue, char * uptime);
};

#endif 