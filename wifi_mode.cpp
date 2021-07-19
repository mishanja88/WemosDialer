#include "wifi_mode.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "eeprom_utils.h"

DNSServer dnsServer;
AsyncWebServer webServer(80);

const char *ssid = "NaborNomera";
const char *password = NULL; // "12345678";


void handleRoot()
{
    webServer.sendHeader("Cache-Control",` "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // here begin chunked transfer
    webServer.send(200, "text/html", "");

    
    /*response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");*/
    
    webServer.sendContent_P(F(R"unicode(
<html>
<head>
<meta charset="utf-8">
<title>Набиратель номера — настройки</title>

<script type="text/javascript">

function setMobile(tab) {
    document.getElementById('prefix').innerHTML = tab;
}

</script>

<style>

table {
    white-space: nowrap;
}

table.main-header {
    spacing: 4;
    padding: 10;
}

table.header-item {
    border: 1px solid;
    spacing: 2;    
    padding: 5;
}

</style>

</head>
<body>

<table class='main-header'>

<tr>
<td>
<h1>Отладка порта RS232</h1>
<pre>
AT
OK
ATDT 123456789;
OK
</pre>
<form action="/port" method="POST" autocomplete="off">
<table class='header-item'>
<tr>
<td width='100%'>
 <input type="text" name="text" value="" placeholder="Команда" required style='width: 100%;'>
</td>
<td>
 <input type="submit" value="Записать">
</td>
<td width="100%">
</td>
</tr>
</table>
</form>
</td>
</tr>

<tr>
<td valign='top'>
<h1>Добавить запись</h1>
<form action="/phonebook" method="POST" autocomplete="off">
<table class='header-item'>
<tr>
 <td>
   № <input type="number" name="index" value="" placeholder="Ячейка" min="0" max="99" required style='width: 6em;'>
 </td>
 <td>
    <input type="text" name="desc" value="" placeholder="Описание" required style='width: 100%;' pattern='[0-9a-zA-Zа-яА-Я \-.,]{0,20}'>
 </td>
<tr>
 <td>
   <input name="isMobile" type="radio" value="true" onclick="setMobile('+7')" checked>Мобильный<br>
   <input name="isMobile" type="radio" value="false" onclick="setMobile('')">Городской<br>
 </td>
 <td>
    <table>
    <tr>
     <td style='min-width: 20px;' align='right'>
      <div id='prefix'>+7</div>
     </td>
     <td>
      <input type="tel" name="phone" value="" placeholder="Телефон" required pattern='[0-9]{0,20}'>
     </td>
    <tr>
    </table>
 </td>
</tr>
<tr>
 <td align='right' colspan='2'>
  <input type="submit" value="Записать">
 </td>
</tr> 
</table>   
</form>

</tr>
<tr>

<td valign='top'>

<h1>Настройки</h1>
<form action="/settings" method="POST" autocomplete="off">
<table class='header-item'>
)unicode"));
    
    DeviceSettings settings = eeprom_read_settings();
    
    {
webServer.sendContent_P(F(R"unicode(     
<tr>
 <td colspan='2'>
   Префикс выхода на мобильную связь<br>
   <input type="text" name="dialPrefix" value="   
)unicode"));

    webServer.sendContent(get_dial_prefix(settings));

    webServer.sendContent_P(F(R"unicode(   
" required style='width: 100%;' pattern='[0-9W,]+'>
   </td>
</tr>
)unicode"));
    }
        
    {
webServer.sendContent_P(F(R"unicode(     
<tr>
 <td colspan='2'>
  Яркость<br>
  <center>0<input type="range" name="brightness" min="0" max="10" value=")unicode"));

    webServer.sendContent(String(settings.brightness, DEC));

    webServer.sendContent_P(F(R"unicode(" step="1">10</center>
 </td>
</tr> 
)unicode"));
    }

    webServer.sendContent_P(F("<tr>"));
    
    {
        webServer.sendContent_P(F(R"unicode( <td>
   Метод набора<br>
   <input name="isPulse" type="radio" value="true" )unicode"));

        if(!settings.isToneDial)
            webServer.sendContent_P(F("checked"));

        webServer.sendContent_P(F(R"unicode(>Импульсный<br>
   <input name="isPulse" type="radio" value="false" )unicode"));

        if(settings.isToneDial)
            webServer.sendContent_P(F("checked"));

        webServer.sendContent_P(F(R"unicode(>Тоновый<br>
 </td>
)unicode"));
    }

    {
        webServer.sendContent_P(F(R"unicode( <td>
   Звук нажатия клавиш<br>
   <input name="isKeySound" type="radio" value="true" )unicode"));

        if(settings.isSoundEnabled)
            webServer.sendContent_P(F("checked"));

        webServer.sendContent_P(F(R"unicode(>Вкл<br>
   <input name="isKeySound" type="radio" value="false" )unicode"));

        if(!settings.isSoundEnabled)
            webServer.sendContent_P(F("checked"));

        webServer.sendContent_P(F(R"unicode(>Выкл<br>
 </td>
)unicode"));
    }
    
webServer.sendContent_P(F(R"unicode( 
</tr>
<tr>
 <td colspan='1'>
 </td>
 <td align='right'>
  <input type="submit" value="Записать">
 </td>
</tr>
</table>   
</form>

</td>
<td width='100%'>
</td>
</tr>
 
<tr>
<td colspan='2'>
<h1>Телефонная книга</h1>

<table border='1' cellspacing='2' cellpadding='5'>
)unicode"));

    for(int idx = 0; idx < PHONEBOOK_SIZE; ++idx)
    {
        char buff[500];
        sprintf(buff, "<tr><td align='center'>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", idx, eeprom_read_printable(idx), eeprom_read_dialable(idx), eeprom_read_desc(idx));
        webServer.sendContent(buff);
    }

webServer.sendContent_P(F(R"unicode(
 </table>
 </td>
</tr>
<tr>
 <td colspan='3'>
  <font size=2>© Миша, 2021</font>
 </td>
</tr>
</table>
</body>
</html>
)unicode"));

    webServer.sendContent(F("")); // this tells web client that transfer is done
    webServer.client().stop();
}


void handleNotFound() 
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }

    webServer.send ( 404, "text/plain", message );
}

void handlePhonebook()
{
    String inputMessage;

    if (webServer.hasArg("index") &&
    webServer.hasArg("isMobile") &&
    webServer.hasArg("desc") &&
    webServer.hasArg("phone")) 
    {
      inputMessage += "index: ";
      inputMessage = webServer.arg("index");
      inputMessage += "isMobile: ";
      inputMessage = webServer.arg("isMobile");
      inputMessage += "desc: ";
      inputMessage = webServer.arg("desc");
      inputMessage += "phone: ";
      inputMessage = webServer.arg("phone");
    }
    else {
      inputMessage = "No message sent";
    }

    request->send(200, "text/html", R"rawliteral(
    <head>
  <meta encoding='UTF-8'>
  <meta http-equiv='refresh' content='5; URL=/'>
  Phonebook : Данные записаны!<br>
)rawliteral" + inputMessage +
R"rawliteral(
</head>
)rawliteral");    
}

void handleSettings()
{
    String inputMessage;

    if (webServer.hasArg("dialPrefix") &&
    webServer.hasArg("brightness") &&
    webServer.hasArg("isPulse") &&
    webServer.hasArg("isKeySound")) 
    {
      inputMessage += "dialPrefix: ";
      inputMessage = webServer.arg("dialPrefix");
      inputMessage += "brightness: ";
      inputMessage = webServer.arg("brightness");
      inputMessage += "isPulse: ";
      inputMessage = webServer.arg("isPulse");
      inputMessage += "isKeySound: ";
      inputMessage = webServer.arg("isKeySound");
    }
    else {
      inputMessage = "No message sent";
    }

    request->send(200, "text/html", R"rawliteral(
    <head>
  <meta encoding='UTF-8'>
  <meta http-equiv='refresh' content='5; URL=/'>
  Settings : Данные записаны!<br>
)rawliteral" + inputMessage +
R"rawliteral(
</head>
)rawliteral");
}

void handlePort()
{
    String inputMessage;

    if (webServer.hasArg("text"))
    {
      inputMessage += "text: ";
      inputMessage = webServer.arg("text");
    }
    else {
      inputMessage = "No message sent";
    }

    request->send(200, "text/html", R"rawliteral(
    <head>
  <meta encoding='UTF-8'>
  <meta http-equiv='refresh' content='5; URL=/'>
  Port : Данные записаны!<br>
)rawliteral" + inputMessage +
R"rawliteral(
</head>
)rawliteral");
}

void wifi_setup(){
  //your other setup stuff...
  WiFi.softAP(ssid);
  dnsServer.start(53, "*", WiFi.softAPIP());
  //server.addHandler(new CaptiveRequestHandler()); //.setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  
    webServer.on("/", handleRoot);
    
    webServer.on("/phonebook", handlePhonebook);
    webServer.on("/settings", handleSettings);
    webServer.on("/port", handlePort);
    
    webServer.onNotFound ( handleNotFound );
    webServer.begin();
}

void wifi_loop(){
    dnsServer.processNextRequest();
    webServer.handleClient();
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
