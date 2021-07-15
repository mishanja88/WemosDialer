#include "wifi_mode.h"

#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);

const char *ssid = "NaborNomera";
const char *password = NULL; // "12345678";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};


void wifi_setup(){
  //your other setup stuff...
  WiFi.softAP(ssid);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()); //.setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
}

void wifi_loop(){
  dnsServer.processNextRequest();
}

/*
#include <DNSServer.h>

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char *ssid = "NaborNomera";
const char *password = NULL; // "12345678";

const byte DNS_PORT = 53;
IPAddress apIP(8, 8, 8, 8);
DNSServer dnsServer;
const char *server_name = "*"; // "www.myesp32.com";  // Can be "*" to all DNS requests

String responseHTML = "<!DOCTYPE html><html>"
                      "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                      "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                      "</style></head>"
                      "<body><h1>ESP32 Web Server</h1>"
                      "<p>Hello World</p>"
                      "</body></html>";


// REPLACE WITH YOUR NETWORK CREDENTIALS
//const char* ssid = "REPLACE_WITH_YOUR_SSID";
//const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* PARAM_INPUT_NUM = "num";
const char* PARAM_INPUT_PHONE = "phone";
const char* PARAM_INPUT_DESC = "desc";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html_head[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <meta charset="utf-8">
  <title>Телефонная книга</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Ячейка: <input type="number" name="num" min="0" max="99">  
    Телефон: <input type="text" name="phone" pattern="[0-9p]{0,20}">
    Имя: <input type="text" name="desc" pattern="[0-9A-Za-zА-Яа-я., \-]{0,30}">
    <input type="submit" value="Записать">
  </form><br>
)rawliteral";

const char index_html_tail[] PROGMEM = R"rawliteral(
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// АЯая
// &#1040;&#1071;&#1072;&#1103;

String getTable()
{
  String result = R"rawliteral(
<table border="1">
<caption>Телефонная книга</caption>
)rawliteral";

  for(int i = 0; i < 100; ++i)
  {
    result += "<tr>";
    for(int j = 0; j < 3; ++j)
    {
    result += "<td>";

    result += String(i, DEC);
    result += ":";
    result += String(j, DEC);

    
    result += "</td>";
    }
    result += "</tr>";
  }

  result += "</table>";
  return result;
}

void wifi_setup()
{
    // Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(100);
  
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // Serial.println();
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String result = String(index_html_head) + getTable() + String(index_html_tail);
    request->send(200, "text/html", result);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_NUM) &&
    request->hasParam(PARAM_INPUT_PHONE) &&
    request->hasParam(PARAM_INPUT_DESC)) 
    {
      inputMessage += "num: ";
      inputMessage = request->getParam(PARAM_INPUT_NUM)->value();
      inputMessage += "phone: ";
      inputMessage = request->getParam(PARAM_INPUT_PHONE)->value();
      inputMessage += "desc: ";
      inputMessage = request->getParam(PARAM_INPUT_DESC)->value();
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/html", R"rawliteral(
    <head>
  <meta http-equiv='refresh' content='2; URL=/'>
  Данные записаны!<br>
)rawliteral" + inputMessage +
R"rawliteral(
</head>
)rawliteral");
  });
  server.onNotFound(notFound);
  server.begin();
  dnsServer.start(DNS_PORT, server_name, apIP);
}

void wifi_loop()
{  
  dnsServer.processNextRequest();
}
*/


bool wifi_is_enabled()
{
  return analogRead(0) > 500;
}

void wifi_disable()
{
   WiFi.mode(WIFI_OFF);
}
