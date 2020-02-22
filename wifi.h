#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

#define AP_NAME "SMART_ENC"
#define AP_PASSWORD "SecurePass"

IPAddress local_IP(192,168,4,1);

IPAddress gateway(192,168,4,4);

IPAddress subnet(255,255,255,0);

/* Init Web Server */
AsyncWebServer server(80);

/* Handler Functions */
void notFound(AsyncWebServerRequest*);
void handleHomePage(AsyncWebServerRequest*);

void wifiSetup(){
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_NAME, AP_PASSWORD);

    /* Setting Up Async Web Server */
    server.on("/", HTTP_GET, handleHomePage);
    server.onNotFound(notFound);("/", HTTP_GET, notFound);
    
}

void handleHomePage(AsyncWebServerRequest *request){
    String page = HOME_PAGE;
    request->send(200, "text/html", page);
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
