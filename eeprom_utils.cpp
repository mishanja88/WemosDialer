#include "eeprom_utils.h"
#include <EEPROM.h>

#define SETTINGS_OFFSET 4000

DeviceSettings eeprom_read_settings()
{
    DeviceSettings result;
    memset(&result, 0, sizeof(DeviceSettings));
    return result;
    
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



String eeprom_read_printable(int idx)
{
    return String();
  
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
    return String();

  
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
    {
        result += get_dial_prefix(settings);
    }
    
    if(entry.num0 > 0)
        result += String(entry.num0, DEC);
    
    if(entry.num1 > 0)
        result += String(entry.num1, DEC);
    
    if(entry.num2 > 0)
        result += String(entry.num2, DEC);
    
    result += ";";
    
    return result;
}

#define ASCII_BORDER 128
#define MIN_RUSSIAN_CHAR 1025
#define MAX_RUSSIAN_CHAR 1105

String eeprom_read_desc(int idx)
{
    return String();

  
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
            result += "&#";
            result += String(MIN_RUSSIAN_CHAR + chr - ASCII_BORDER, DEC);
            result += ";";          
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
    
    String s0, s1, s2;
    
    if(number.length() > 0)
        s0 = number.substring(0, max(number.length(), 9u));    
    
    if(number.length() > 9)
        s1 = number.substring(9, max(number.length(), 18u));    
    
    if(number.length() > 18)
        s2 = number.substring(18, max(number.length(), 27u));    
    
    
    entry.num0 = s0.toInt();
    entry.num1 = s1.toInt(); 
    entry.num2 = s2.toInt();
    
    int strIdx = 0;
    for(int i = 0; i < ENTRY_DESC_SIZE; ++i)
    {
        char dest_chr = '\0';
        if(strIdx < desc.length())
        {
            char src_chr = desc.charAt(strIdx);
            
            if(src_chr == '&') // russian
            {
                // &#1025;
                if(strIdx + 7 > desc.length())
                {
                    strIdx = desc.length(); // abort parsing
                }
                else
                {
                    long i_code = desc.substring(strIdx + 2, strIdx + 6).toInt();
                    if(i_code < MIN_RUSSIAN_CHAR || i_code > MAX_RUSSIAN_CHAR)
                    {
                        strIdx = desc.length(); // abort parsing    
                    }
                    else // ok
                    {
                        dest_chr = ASCII_BORDER + i_code - MIN_RUSSIAN_CHAR;
                        strIdx += 7;
                    }                                
                }
            }
            else // latin
            {
                if(src_chr >= ASCII_BORDER)
                    strIdx = desc.length(); // abort parsing
                    else
                    {
                        dest_chr = src_chr; // ok
                        strIdx++;
                    }
            }
        }
        
        entry.desc[i] = dest_chr;
    }    
    
    EEPROM.put(sizeof(BookEntry) * idx, entry);
    EEPROM.commit();    
}
