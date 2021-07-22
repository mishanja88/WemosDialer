#ifndef __EEPROM_UTILS_H
#define __EEPROM_UTILS_H

#include <Arduino.h>

#pragma pack(push, 1)

#define DIAL_PREFIX_SIZE 10
struct DeviceSettings
{
   bool isSoundEnabled : 1;
   bool isToneDial : 1;
   uint8_t extraDelay : 6;
   
   uint8_t brightness;
   char dialPrefix[ DIAL_PREFIX_SIZE ];
};

String get_dial_prefix(const DeviceSettings &settings);

#define PHONEBOOK_SIZE 100

#define ENTRY_NUM_SIZE 14
#define ENTRY_DESC_SIZE 25
struct BookEntry
{
  bool isMobile : 1; 
  uint8_t reserve : 7;
  
  uint8_t num[ ENTRY_NUM_SIZE ];

  char desc[ ENTRY_DESC_SIZE ];
};

#pragma pack(pop)


DeviceSettings eeprom_read_settings();

void eeprom_write_settings(DeviceSettings val);

String eeprom_read_printable(int idx);

String eeprom_read_dialable(int idx);

String eeprom_read_desc(int idx);

void eeprom_write_entry(int idx, bool isMobile, String number, String desc);


#endif // __EEPROM_UTILS_H
