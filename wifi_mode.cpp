#include "wifi_mode.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "port.h"
#include "eeprom_utils.h"
#include "wifi_help.h"

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char *ssid = "NaborNomera";
const char *password = NULL; // "12345678";

void handleHelp()
{  
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // here begin chunked transfer
    webServer.send(200, "text/html", "");

    webServer.sendContent((__FlashStringHelper *) help_html);

    webServer.sendContent(F("")); // this tells web client that transfer is done
    webServer.client().stop();    
}
void handleRoot()
{  
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // here begin chunked transfer
    webServer.send(200, "text/html", "");
    
    webServer.sendContent(F("" \
"\n<html>" \
"\n<head>" \
"\n<meta charset='utf-8'>" \
"\n<title>Набиратель номера</title>" \
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
"\n <h1>Справка</h1>" \
"\n <a href='/help'>Нажмите здесь, чтобы перейти на страницу справки</a>" \
"\n </td>" \
"\n</tr>" \
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
"\n    <input type='text' name='desc' value='' placeholder='Описание' required style='width: 100%;' pattern='[\\x00-\\x7Fа-яА-ЯёЁ]{0,25}'>" \
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

    webServer.sendContent(F(" required style='width: 100%;' pattern='[0-9W,]{0,10}'>" \
"\n </td>" \
"\n</tr>"));
    }
    
    {
    webServer.sendContent(F("\n" \
    "\n<tr>" \
    "\n <td colspan='2'>Дополнительная задержка<br>после набора номера, 0-63 секунд<br>" \
    "\n <input type='number' name='extraDelay' min='0' max='63' value="));

    webServer.sendContent("'" + String(settings.extraDelay, DEC) + "'");

    webServer.sendContent(F(" required style='width: 100%;'>" \
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
        sprintf(buff, "\n <tr><td align='center'>%d</td><td>%s</td><td>%s</td></tr>\n",
                    idx, 
                    eeprom_read_printable(idx).c_str(), // eeprom_read_dialable(idx).c_str(), 
                    eeprom_read_desc(idx).c_str());
                    
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
      inputMessage += "<br> index: ";
      inputMessage += webServer.arg(PARAM_INPUT_NUM);
      inputMessage += "<br> isMobile: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_MOBILE);
      inputMessage += "<br> desc: ";
      inputMessage += webServer.arg(PARAM_INPUT_DESC);
      inputMessage += "<br> phone: ";
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
const char* PARAM_INPUT_EXTRADELAY = "extraDelay";

void handleSettings()
{
    String inputMessage;

    if (webServer.hasArg(PARAM_INPUT_DIALPREFIX) &&
    webServer.hasArg(PARAM_INPUT_BRIGHTNESS) &&
    webServer.hasArg(PARAM_INPUT_IS_PULSE) &&
    webServer.hasArg(PARAM_INPUT_IS_KEYSOUND) &&
    webServer.hasArg(PARAM_INPUT_EXTRADELAY)) 
    {
      inputMessage += "<br> dialPrefix: ";
      inputMessage += webServer.arg(PARAM_INPUT_DIALPREFIX);
      inputMessage += "<br> extraDelay: ";
      inputMessage += webServer.arg(PARAM_INPUT_EXTRADELAY);
      inputMessage += "<br> brightness: ";
      inputMessage += webServer.arg(PARAM_INPUT_BRIGHTNESS);
      inputMessage += "<br> isPulse: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_PULSE);
      inputMessage += "<br> isKeySound: ";
      inputMessage += webServer.arg(PARAM_INPUT_IS_KEYSOUND);

      DeviceSettings val;
      strncpy(val.dialPrefix, webServer.arg(PARAM_INPUT_DIALPREFIX).c_str(), DIAL_PREFIX_SIZE);
      val.brightness = webServer.arg(PARAM_INPUT_BRIGHTNESS).toInt();
      val.extraDelay = webServer.arg(PARAM_INPUT_EXTRADELAY).toInt();
      val.isSoundEnabled = (webServer.arg(PARAM_INPUT_IS_KEYSOUND) == String("true"));
      val.isToneDial = (webServer.arg(PARAM_INPUT_IS_PULSE) == String("false"));
      
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

    MDNS.begin("nn");

    const char *roots[] = {
      "/", "/generate_204", "/fwlink", "/favicon.ico",
      "/connect", "/kindle-wifi/wifistub.html",
      "/connecttest.txt", "/hotspot-detect.html",
      "/library/test/success.html",
      NULL
      };

    for(int i = 0; roots[i] != NULL; ++i)
      webServer.on(roots[i], handleRoot);
    
    webServer.on("/help", handleHelp);
    webServer.on("/phonebook", handlePhonebook);
    webServer.on("/settings", handleSettings);
    webServer.on("/port", handlePort);
    
    
    webServer.onNotFound ( handleNotFound );
    webServer.begin();
        
  MDNS.addService("http", "tcp", 80);
}

void wifi_loop()
{
    MDNS.update();
    dnsServer.processNextRequest();
    webServer.handleClient();

    port_update_buffer(200);
}

bool wifi_is_enabled()
{
  return analogRead(0) > 500;
}

void wifi_disable()
{
   WiFi.mode(WIFI_OFF);
}
