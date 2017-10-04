/*
 * Copyright (C) 2016 David McKelvie.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

///
///\file      ds18b20.c
///
///\brief      Dallas/Maxim DS18B20 1-wire digital temperature sensor
///            driver
///
///\author      <A HREF="http://www.subverse.co.nz">David McKelvie</A>
///
///\version      1.1 10/10/2010
///            - formatted for doxygen
///
///\version      1.0
///
#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include "onewire.h"
#include "ds18b20.h"

//#define DEBUG          1

static uint8_t device_count;
static ds18b20_settings_t ds18b20[MAX_TEMP_SENSORS];

///
///\brief   find and initialise ds18b20 sensors
///
///\return   number of ds18b20s found
///
uint8_t ds18b20_init(void)
{
   uint8_t i;
   device_count = 0;
   memset(ds18b20, 0, sizeof(ds18b20_settings_t) * MAX_TEMP_SENSORS);
   ow_init_rom_search();

   for (i = 0; i < MAX_TEMP_SENSORS; i++)
   {
      do
      {
         // find a 1-wire device
         if (ow_find_device(ds18b20[i].rom))
         {
            // is it a DS18B20
            if (ds18b20[i].rom[ DS_FAMILY_CODE_INDEX] == DS18B20_FAMILY_CODE)
            {
               device_count++;
               if (ow_match_rom(ds18b20[i].rom))
               {
                  ow_write_byte(DS_COMMAND_WRITE_SCRATCHPAD);
                  ow_write_byte(120);                     // Th
                  ow_write_byte(0);                        // Tl
                  ow_write_byte(0x1F);                     // 9-bit resolution
               }
            }
            if (ow_last_device_found())
               return device_count;
         }
         else
            // no more 1-wire devices
            return device_count;

      } while (ds18b20[i].rom[DS_FAMILY_CODE_INDEX] != DS18B20_FAMILY_CODE);

      if (i < MAX_TEMP_SENSORS)
         memcpy(ds18b20 + i + 1, ds18b20 + i, sizeof(ds18b20_settings_t));
   }
   return device_count;
}

///
///\brief   Read temperature from ds18b20
///
///\details   Start a temperature conversion on a given device,
///         wait for the conversion, convert result to 8 bit
///         signed int and return it.
///
///\param   device index to known ds18b20s
///
///\return   temperature
///
int8_t ds18b20_read_temp(uint8_t device)
{
   int8_t temperature;
   uint8_t buffer[2] = {0};

   if (ds18b20_read_temp_raw(device, buffer))
   {
     temperature = (buffer[0] >> 4) & 0x0F;         // convert to 8 bit signed int
     if (buffer[0] & 0x08)
        temperature++;
     temperature |= (buffer[1] << 4) & 0xF0;

     return temperature;
   }
   return 0;
}

///
///\brief   Read temperature from ds18b20
///
///\details   Start a temperature conversion on a given device,
///         wait for the conversion, return raw sensor value
///
///\param   device index to known ds18b20s
///\param   buffer to store result
///
///\return   non-zero on success
///
uint8_t ds18b20_read_temp_raw(uint8_t device, uint8_t *buffer)
{
   if (device < device_count)
   {
      if (ow_match_rom(ds18b20[device].rom))      // select device
      {
         ow_write_byte( DS_COMMAND_CONVERT);         // start conversion
         while (!ow_read_bit())                     // wait for conversion
            _delay_ms(10);

         if (ow_match_rom(ds18b20[device].rom))
         {
            ow_write_byte(DS_COMMAND_READ_SCRATCHPAD);

            buffer[0] = ow_read_byte();                  // read raw result
            buffer[1] = ow_read_byte();

            ow_reset();

            return 1;
         }
      }
   }
   return 0;
}

///
///\brief   Verify ds18b20 ROM CRC
///
///\param   device index to known ds18b20s
///
///\return   non-zero if given device's ROM CRC is correct.
///
uint8_t ds18b20_verify_crc(uint8_t device)
{
   if (device < device_count)
      return !ow_check_crc(ds18b20[device].rom);
   return 0;
}
