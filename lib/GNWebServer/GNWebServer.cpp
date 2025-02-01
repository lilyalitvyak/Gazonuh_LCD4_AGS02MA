#include "GNWebServer.h"

GNWebServer::GNWebServer()
:server{8888} 
{
    ssid = "Tenda_AX3000_2G";  
    password = "TarakanovnaMojaTarakanovna"; 

    Serial.begin(115200);
    Serial.println(__FILE__);
    Serial.println();
}

void GNWebServer::update(uint32_t ppbValue, char * uptime) {
    Serial.print("----> Send PPBValue ");
    Serial.println(ppbValue);
    Serial.print("----> Send Uptime ");
    Serial.println(uptime);
};
