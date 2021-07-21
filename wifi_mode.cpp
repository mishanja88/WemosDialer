#include "wifi_mode.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "port.h"
#include "eeprom_utils.h"

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char *ssid = "NaborNomera";
const char *password = NULL; // "12345678";


void handleRoot()
{  
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
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
    
    webServer.sendContent(F("" \
"\n<html>" \
"\n<head>" \
"\n<meta charset='utf-8'>" \
"\n<title>Набиратель номера — настройки</title>" \
"\n" \
"\n<script type='text/javascript'>" \
"\n" \
"\nfunction setMobile(tab) {" \
"\n    document.getElementById('prefix').innerHTML = tab;" \
"\n}" \
"\n" \
"\n</script>" \
"\n" \
"\n<style>" \
"\n" \
"\ntable {" \
"\n    white-space: nowrap;" \
"\n}" \
"\n" \
"\ntable.main-header {" \
"\n    spacing: 4;" \
"\n    padding: 10;" \
"\n}" \
"\n" \
"\ntable.header-item {" \
"\n    border: 1px solid;" \
"\n    spacing: 2;" \
"\n    padding: 5;" \
"\n}" \
"\n" \
"\n</style>" \
"\n" \
"\n</head>" \
"\n<body>" \
"\n" \
"\n<table class='main-header'>" \
"\n" \
"\n<tr>" \
"\n <td>" \
"\n <h1>Отладка порта RS232</h1>" \
"\n "));

webServer.sendContent(String("<pre>") + port_get_buffer() + String("</pre>"));

webServer.sendContent(F("\n <form action='/port' method='POST' autocomplete='off' accept-charset='windows-1251'>" \
"\n <table class='header-item'>" \
"\n  <tr>" \
"\n  <td width='100%'>" \
"\n   <input type='text' name='text' value='' placeholder='Команда' required style='width: 100%;'>" \
"\n  </td>" \
"\n  <td>" \
"\n   <input type='submit' value='Записать'>" \
"\n  </td>" \
"\n  <td width='100%'>" \
"\n  </td>" \
"\n </tr>" \
"\n </table>" \
"\n </form>" \
"\n </td>" \
"\n</tr>" \
"\n" \
"\n<tr>" \
"\n <td valign='top'>" \
"\n <h1>Добавить запись</h1>" \
"\n <form action='/phonebook' method='POST' autocomplete='off' accept-charset='windows-1251'>" \
"\n <table class='header-item'>" \
"\n  <tr>" \
"\n  <td>" \
"\n   № <input type='number' name='index' value='' placeholder='Ячейка' min='0' max='99' required style='width: 6em;'>" \
"\n  </td>" \
"\n  <td>" \
"\n    <input type='text' name='desc' value='' placeholder='Описание' required style='width: 100%;' pattern='[\\x00-\\x7Fа-яА-ЯёЁ]{0,28}'>" \
"\n  </td>" \
"\n  </tr>" \
"\n  <tr>" \
"\n <td>" \
"\n   <input name='isMobile' type='radio' value='true' onclick=\"setMobile('+7')\" checked>Мобильный<br>" \
"\n   <input name='isMobile' type='radio' value='false' onclick=\"setMobile('')\">Городской<br>" \
"\n </td>" \
"\n <td>" \
"\n    <table>" \
"\n    <tr>" \
"\n     <td style='min-width: 20px;' align='right'>" \
"\n      <div id='prefix'>+7</div>" \
"\n     </td>" \
"\n     <td>" \
"\n      <input type='tel' name='phone' value='' placeholder='Телефон' required pattern='[0-9]{0,28}'>" \
"\n     </td>" \
"\n    </tr>" \
"\n    </table>" \
"\n </td>" \
"\n </tr>" \
"\n <tr>" \
"\n <td align='right' colspan='2'>" \
"\n  <input type='submit' value='Записать'>" \
"\n </td>" \
"\n </tr>" \
"\n</table>" \
"\n</form>" \
"\n</tr>"));

    webServer.sendContent(F("" \
"\n<tr>" \
"\n" \
"\n<td valign='top'>" \
"\n" \
"\n<h1>Настройки</h1>" \
"\n<form action='/settings' method='POST' autocomplete='off' accept-charset='windows-1251'>" \
"\n<table class='header-item'>" \
"\n"));

    DeviceSettings settings = eeprom_read_settings();
    
    {
    webServer.sendContent(F("\n" \
"\n<tr>" \
"\n <td colspan='2'>Префикс выхода на мобильную связь<br>" \
"\n <input type='text' name='dialPrefix' value="));

    webServer.sendContent("'" + get_dial_prefix(settings) + "'");

    webServer.sendContent(F(" required style='width: 100%;' pattern='[0-9W,]+'>" \
"\n </td>" \
"\n</tr>"));
    }
        
    {
webServer.sendContent(F("\n<tr><td colspan='2'>Яркость<br><center>" \
"\n 0 <input type='range' name='brightness' min='0' max='10' value="));

    webServer.sendContent("'" + String(settings.brightness, DEC) + "'");

    webServer.sendContent(F(" step='1'> 10 " \
"\n </center></td></tr>"));
    }

    webServer.sendContent(F("\n <tr>"));
    
    {
        webServer.sendContent(F("\n <td>Метод набора<br><input name='isPulse' type='radio' value='true' "));

        if(!settings.isToneDial)
            webServer.sendContent(F("checked"));

        webServer.sendContent(F(" >Импульсный<br>\n <input name='isPulse' type='radio' value='false' "));

        if(settings.isToneDial)
            webServer.sendContent(F("checked"));

        webServer.sendContent(F(" >Тоновый<br></td>"));
    }

    {
webServer.sendContent(F("\n <td>Звук нажатия клавиш<br><input name='isKeySound' type='radio' value='true' "));

        if(settings.isSoundEnabled)
            webServer.sendContent(F("checked"));


        webServer.sendContent(F(" >Вкл<br>\n <input name='isKeySound' type='radio' value='false' "));

        if(!settings.isSoundEnabled)
            webServer.sendContent(F("checked"));

        webServer.sendContent(F(" >Выкл<br></td>"));
    }

webServer.sendContent(F("\n " \
"\n " \
"\n </tr>" \
"\n <tr>" \
"\n  <td colspan='1'>" \
"\n  </td>" \
"\n  <td align='right'>" \
"\n   <input type='submit' value='Записать'>" \
"\n  </td>" \
"\n </tr>" \
"\n </table>" \
"\n </form>" \
"\n " \
"\n </td>" \
"\n <td width='100%'>" \
"\n \n </td>" \
"\n </tr>"));

webServer.sendContent(F("" \
"\n  " \
"\n <tr>" \
"\n <td colspan='2'>" \
"\n <h1>Телефонная книга</h1>" \
"\n " \
"\n <table border='1' cellspacing='2' cellpadding='5'>" \
""));

    for(int idx = 0; idx < PHONEBOOK_SIZE; ++idx)
    {
        char buff[500];
        sprintf(buff, "\n <tr><td align='center'>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", idx, eeprom_read_printable(idx).c_str(), eeprom_read_dialable(idx).c_str(), eeprom_read_desc(idx).c_str());
        webServer.sendContent(buff);
    }

webServer.sendContent(F("\n </table>" \
"\n  </td>" \
"\n </tr>" \
"\n <tr>" \
"\n  <td colspan='3'>" \
"\n   <font size=2>© Миша, 2021</font>" \
"\n  </td>" \
"\n </tr>" \
"\n </table>" \
"\n </body>" \
"\n </html>" \
""));

    webServer.sendContent(F("")); // this tells web client that transfer is done
    webServer.client().stop();
}


void handleNotFound() 
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += webServer.uri();
    message += "\nMethod: ";
    message += ( webServer.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += webServer.args();
    message += "\n";

    for ( uint8_t i = 0; i < webServer.args(); i++ ) {
        message += " " + webServer.argName ( i ) + ": " + webServer.arg ( i ) + "\n";
    }

    webServer.send ( 404, "text/plain", message );
}

const char* PARAM_INPUT_NUM = "index";
const char* PARAM_INPUT_DESC = "desc";
const char* PARAM_INPUT_IS_MOBILE = "isMobile";
const char* PARAM_INPUT_PHONE = "phone";


void handlePhonebook()
{
    String inputMessage;

    if (webServer.hasArg(PARAM_INPUT_NUM) &&
    webServer.hasArg(PARAM_INPUT_DESC) &&
    webServer.hasArg(PARAM_INPUT_IS_MOBILE) &&
    webServer.hasArg(PARAM_INPUT_PHONE)) 
    {
      inputMessage += "<br>index: ";
      inputMessage += webServer.arg(PARAM_INPUT_NUM);
      inputMessage += "<br>isMobile: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_MOBILE);
      inputMessage += "<br>desc: ";
      inputMessage += webServer.arg(PARAM_INPUT_DESC);
      inputMessage += "<br>phone: ";
      inputMessage += webServer.arg(PARAM_INPUT_PHONE);


      bool isMobile = (webServer.arg(PARAM_INPUT_IS_MOBILE) == String("true"));

      eeprom_write_entry(webServer.arg(PARAM_INPUT_NUM).toInt(), \
              isMobile, \
              webServer.arg(PARAM_INPUT_PHONE), \
              webServer.arg(PARAM_INPUT_DESC));
    }
    else {
      inputMessage = "No message sent";
    }

    webServer.send(200, "text/html", \
"    <head> " \
"    <meta charset='windows-1251'>" \
"  <meta http-equiv='refresh' content='3; URL=/'>" \
"  Phonebook : OK!<br>" \
 + inputMessage + \
"</head>" \
);    
}


const char* PARAM_INPUT_DIALPREFIX = "dialPrefix";
const char* PARAM_INPUT_BRIGHTNESS = "brightness";
const char* PARAM_INPUT_IS_PULSE = "isPulse";
const char* PARAM_INPUT_IS_KEYSOUND = "isKeySound";

void handleSettings()
{
    String inputMessage;

    if (webServer.hasArg(PARAM_INPUT_DIALPREFIX) &&
    webServer.hasArg(PARAM_INPUT_BRIGHTNESS) &&
    webServer.hasArg(PARAM_INPUT_IS_PULSE) &&
    webServer.hasArg(PARAM_INPUT_IS_KEYSOUND)) 
    {
      inputMessage += "dialPrefix: ";
      inputMessage += webServer.arg(PARAM_INPUT_DIALPREFIX);
      inputMessage += "brightness: ";
      inputMessage += webServer.arg(PARAM_INPUT_BRIGHTNESS);
      inputMessage += "isPulse: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_PULSE);
      inputMessage += "isKeySound: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_KEYSOUND);

      DeviceSettings val;
      strncpy(val.dialPrefix, webServer.arg(PARAM_INPUT_DIALPREFIX).c_str(), DIAL_PREFIX_SIZE);
      val.brightness = webServer.arg(PARAM_INPUT_BRIGHTNESS).toInt();
      val.isSoundEnabled = (webServer.arg(PARAM_INPUT_IS_KEYSOUND) == String("true"));
      val.isToneDial = (webServer.arg(PARAM_INPUT_IS_PULSE) == String("false"));
      val.reserve = 0;
      
      eeprom_write_settings(val);
    }
    else {
      inputMessage = "No message sent";
    }

    webServer.send(200, "text/html", \
"    <head> " \
"    <meta charset='windows-1251'>" \
"  <meta http-equiv='refresh' content='3; URL=/'>" \
"  Settings : OK!<br>" \
 + inputMessage + \
"</head>" \
);    
}

const char* PARAM_INPUT_TEXT = "text";

void handlePort()
{
    String inputMessage;

    if (webServer.hasArg(PARAM_INPUT_TEXT))
    {
      inputMessage += "text: ";

      String text = webServer.arg(PARAM_INPUT_TEXT);
      inputMessage += text;

      int idx = text.toInt();
      if(text == "0" || idx != 0)
         port_dial_debug_to_buffer(idx);
      else
         port_send(text);
    }
    else {
      inputMessage = "No message sent";
    }

//"  <meta charset='UTF-8'>" \

    webServer.send(200, "text/html", \
"    <head> " \
"    <meta charset='windows-1251'>" \
"  <meta http-equiv='refresh' content='3; URL=/'>" \
"  Port : OK!<br>" \
 + inputMessage + \
"</head>" \
);    
}

void wifi_setup()
{
    WiFi.hostname("nn");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
  
    delay(100);
  
    IPAddress apIP(8, 8, 8, 8);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());
  
    webServer.on("/", handleRoot);
    webServer.on("/generate_204", handleRoot); //Android captive portal
    webServer.on("/fwlink", handleRoot); //Microsoft captive portal
    webServer.on("/favicon.ico", handleRoot); //Another captive portal
    
    
    webServer.on("/phonebook", handlePhonebook);
    webServer.on("/settings", handleSettings);
    webServer.on("/port", handlePort);
    
    
    webServer.onNotFound ( handleNotFound );
    webServer.begin();
}

void wifi_loop()
{
    dnsServer.processNextRequest();
    webServer.handleClient();

    port_update_buffer(200);
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
