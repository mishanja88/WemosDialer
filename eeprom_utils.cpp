#include "eeprom_utils.h"
#include <EEPROM.h>

#define SETTINGS_OFFSET 4000

DeviceSettings eeprom_read_settings()
{
    DeviceSettings result;
    EEPROM.get(SETTINGS_OFFSET, result);
    
    if(result.brightness > 10)
        result.brightness = 10;
    
    for(int i = 0; i < DIAL_PREFIX_SIZE; ++i)
    {
        switch(result.dialPrefix[i])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'W':
            case ',':
                continue;
                
            default:
                result.dialPrefix[i] = '\0';
        }
        
        break;
    }
    
    return result;
}

void eeprom_write_settings(DeviceSettings val)
{
    EEPROM.put(SETTINGS_OFFSET, val);
    EEPROM.commit();
}

String read_number(const BookEntry &entry)
{
    String result;
    for(int i = 0; i < ENTRY_NUM_SIZE; ++i)
    {
       uint8_t src0 = entry.num[i] >> 4;
       uint8_t src1 = entry.num[i] & 0xF;
       if(src0 >= 10)
          break;
       else
          result += char('0' + src0);

       if(src1 >= 10)
          break;
       else
          result += char('0' + src1);
    }
    
    return result;  
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

    result += read_number(entry);
    
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
    
    if(entry.isMobile)
        result += get_dial_prefix(settings);
    
    result += read_number(entry);
    
    result += ";";
    
    return result;
}

#define ASCII_BORDER 128
#define MIN_RUSSIAN_CHAR 1040
#define MAX_RUSSIAN_CHAR 1105
#define WIN1151_CYR 192

String eeprom_read_desc(int idx)
{  
    if (idx < 0 || idx >= PHONEBOOK_SIZE)
        return String();
    
    BookEntry entry;
    EEPROM.get(sizeof(BookEntry) * idx, entry);
    
    String result;
    result.reserve(ENTRY_DESC_SIZE);
    
    for(int i = 0; i < ENTRY_DESC_SIZE; ++i)
    {
        char chr = entry.desc[i];
        if(chr == '\0')
            break;
        
        if(chr < ASCII_BORDER) // latin
        {
            result += chr;
        }
        else // russian: unicode 1025 ... 1105
        {
            if(chr >= WIN1151_CYR)
               result += "&#" + String(MIN_RUSSIAN_CHAR + chr - WIN1151_CYR, DEC) + ";";
            else
            {
              switch(chr)
              {
                case 168: result += "Ё"; break;
                case 184: result += "ё"; break;
                default: result += " "; break;
              }
            }
        }
    }
    
    return result;
}

String get_dial_prefix(const DeviceSettings &settings)
{
  char *str = new char[DIAL_PREFIX_SIZE + 1];
  memset(str, 0, DIAL_PREFIX_SIZE + 1);
  memcpy(str, settings.dialPrefix, DIAL_PREFIX_SIZE);
  String result(str);
  delete [] str;
  return result;
}

void eeprom_write_entry(int idx, bool isMobile, String number, String desc)
{
    if (idx < 0 || idx >= PHONEBOOK_SIZE)
        return;
    
    BookEntry entry;
    entry.isMobile = isMobile;

    memset(entry.num, 0xFF, ENTRY_NUM_SIZE);

    int dest_idx = 0;
    for(int i = 0; i < number.length() && dest_idx < ENTRY_NUM_SIZE; ++i, ++dest_idx)
    {
       char chr = number.charAt(i);
       if(chr < '0' || chr > '9')
          break;

       uint8_t dest_chr = (chr - '0') << 4;
       ++i;

       if(i < number.length())
       {
          chr = number.charAt(i);
          if(chr < '0' || chr > '9')
            dest_chr |= 0xF;
          else
            dest_chr |= (chr - '0');
       }
       else
       {
           dest_chr |= 0xF;
       }

       entry.num[dest_idx] = dest_chr;
    }

    strncpy(entry.desc, desc.c_str(), ENTRY_DESC_SIZE);
    
    EEPROM.put(sizeof(BookEntry) * idx, entry);
    EEPROM.commit();    
}
