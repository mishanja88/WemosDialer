#include "eeprom_utils.h"
#include <EEPROM.h>

#define SETTINGS_OFFSET 4000
#define PHONEBOOK_SIZE 100

DeviceSettings eeprom_read_settings()
{
  DeviceSettings result;
  EEPROM.get(SETTINGS_OFFSET, result);
  return result;
}

void eeprom_write_settings(DeviceSettings val)
{
  EEPROM.put(SETTINGS_OFFSET, val);
}



String eeprom_read_printable(int idx)
{
  if (idx < 0 || idx >= PHONEBOOK_SIZE)
    return String();

  BookEntry entry;
  EEPROM.get(sizeof(BookEntry) * idx, entry);

  String result;
  if(entry.isMobile)
  {
    result += "+7 ";
  }

  if(entry.num0 > 0)
    result += String(entry.num0, DEC);

  if(entry.num1 > 0)
    result += String(entry.num1, DEC);

  if(entry.num2 > 0)
    result += String(entry.num2, DEC);

  return result;
}

String eeprom_read_dialable(int idx)
{
  if (idx < 0 || idx >= PHONEBOOK_SIZE)
    return String();

  String result = "ATD";

  DeviceSettings settings;
  EEPROM.get(SETTINGS_OFFSET, settings);

  if(settings.isToneDial)
    result += "T";
  else
    result += "P";

  result += " ";
  
  BookEntry entry;
  EEPROM.get(sizeof(BookEntry) * idx, entry);

  String result;
  if(entry.isMobile)
  {
    result += "+7 ";
  }

  if(entry.num0 > 0)
    result += String(entry.num0, DEC);

  if(entry.num1 > 0)
    result += String(entry.num1, DEC);

  if(entry.num2 > 0)
    result += String(entry.num2, DEC);

  return result;
}

String eeprom_read_desc(int idx)
{
  if (idx < 0 || idx >= PHONEBOOK_SIZE)
    return String();

  BookEntry entry;
  EEPROM.get(sizeof(BookEntry) * idx, entry);
}

void eeprom_write_entry(int idx, bool isMobile, String number, String desc)
{
  if (idx < 0 || idx >= PHONEBOOK_SIZE)
    return;
}
