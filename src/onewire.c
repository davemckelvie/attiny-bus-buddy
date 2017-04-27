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
///\file   onewire.c
///
///\brief   1 wire interface
///
///\author   <A HREF="http://www.subverse.co.nz">David McKelvie</A>
///
///\version   1.1 10/10/2010
///         - formatted for doxygen
///
///\version 1.0
///
///\details
/// - ROM Search API implementation of AN187
///  -# call ow_init_rom_search()
///  -# call ow_find_device() with a pointer to an 8 byte string (ROM_ID).
///  -# if ow_find device() returns 1 save string as it contains a valid ROM_ID.
///  -# call ow_last_device_found(), if it returns 0 more devices are on the 1-wire bus.
///  -# to find next device call ow_find_device() with a copy of the last found ROM_ID, this gets over-written with new ROM_ID.
///  -# to find all the devices on the 1-wire bus repeat steps 4 and 5 until ow_last_device_found() returns 1.
///
///\verbatim
/// ow_find_device()    ow_last_device_found()  Meaning
/// ---------------------------------------------------
/// 0                   --                      No device found.
/// 1                   0                       Device found, more to find.
/// 1                   1                       Device found, no more to find.
///\endverbatim
///
#include <stdint.h>
#include <avr/io.h>
#include <util/crc16.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "onewire.h"

static uint8_t lastDeviceFound;
static uint8_t lastConflict;

///
///\brief Initialise ROM ID search variables
///
uint8_t ow_init_rom_search(void)
{
   lastDeviceFound = 0;
   lastConflict = 0;
   return 1;
}

///
///\brief Find the next 1 wire device on the bus
///
///\details Find the next device using the given ROM ID. This will
///         find the next leaf using a binary tree search. If the
///         given ROM ID is all 0s then this will find the first device.
///
///\param   rom pointer to 8 byte array to hold the found ROM ID
///
///\return   1 when device found
///
uint8_t ow_find_device(uint8_t *rom)
{
   int8_t i, j, k, currentBit, currentConflict, lastZero, writeBit;
   currentBit = 1;
   lastZero = 0;
   if (ow_reset())
   {
      ow_write_byte(OW_COMMAND_SEARCH_ROM);
      for (i = 7; i >= 0; i--)               // for each bit of rom code from LSB to MSB
      {
         for (j = 0; j < 8; j++)
         {
            k = 0;
            if (ow_read_bit())
               k = 0x01;      // ID bit
            if (ow_read_bit())
               k |= 0x02;      // ID bit compliment

            writeBit = 0;
            switch (k)
            {
               case 0x00:                     // devices have conflicting ROM code bits.
                  if (currentBit == lastConflict)    // We have reached the last conflict...
                     writeBit = 1;               // ...so take '1' path this time.
                  else
                  {
                     if (currentBit > lastConflict)   // We have found more conflicts...
                     {
                        currentConflict = currentBit;   // ...so stay on the '0' path.
                        lastZero = currentBit;
                     }
                     else                     // We have not yet reached the last
                     {                        // ...conflict take the same path as
                        if (*(rom + i) & _BV(j))   //...last time.
                           writeBit = 1;
                        else
                           lastZero = currentBit;
                     }
                  }
                  break;

               case 0x01:               // all devices have '1' in this ROM code bit
                  writeBit = 1;            // keep devices connected to bus
                  break;

               case 0x02:               // all devices have '0' in this ROM code bit
                  break;

               case 0x03:               // no devices on bus
               default:               // ...or error
                  lastDeviceFound = 1;
                  return 0;
            }

            if (writeBit)
            {
               *(rom + i) |= (1 << j);
               ow_write_bit(1);
            }
            else
            {
               *(rom + i) &= ~(1 << j);
               ow_write_bit(0);
            }

            currentBit++;
         }
      }
      lastConflict = lastZero;      // this is used next time
      if (!lastConflict)
         lastDeviceFound = 1;
      return !ow_check_crc(rom);
   }
   return 0;                  // no response to ow_reset()
}

///
///\brief Determine whether last device on bus has been found
///
///\return non-zero when last device has been found
///
uint8_t ow_last_device_found(void)
{
   return lastDeviceFound;
}

///
///\brief 10 microsecond delay
///
///\param delay the number of 10 microsecond delays required
///
void delay_10us(unsigned int delay)
{
   while (delay--)
      _delay_us(10);
}

///
///\brief Reset 1-Wire bus.
///
///\return non-zero if presence pulse detected
///
uint8_t ow_reset(void)
{
   uint8_t tmp;
   cli();
   OW_ASSERT();            // force pin low
   delay_10us(48);         // AN126 H
   OW_RELEASE();
   delay_10us(7);         // AN126 I
   tmp = bit_is_clear(OW_PINS, OW_PIN);   // sample presence pulse
   delay_10us(41);         // AN126 J
   sei();
   return tmp;
}

///
///\brief Read single bit from 1-Wire bus
///
///\return non-zero when bit set
///
uint8_t ow_read_bit(void)
{
   uint8_t tmp;
   cli();
   OW_DDR_OUT();            // make pin output
   OW_PIN_LOW();            // force pin low
   _delay_us(1);
   OW_DDR_IN();            // release pin
   OW_PIN_HIGH();            // enable pullup
   _delay_us(6);
   tmp = bit_is_set(OW_PINS, OW_PIN);
   delay_10us(6);
   sei();
   return tmp;
}

///
///\brief Write single bit to 1-Wire bus
///
///\param bitval value to write to bus
///
uint8_t ow_write_bit(uint8_t bitval)
{
   cli();
   OW_DDR_OUT();            // make pin output
   OW_PIN_LOW();            // force pin low
   _delay_us(6);            // AN126 A
   if (bitval)
      OW_DDR_IN();         // release pin
   delay_10us(5);         // AN126 C
   OW_DDR_IN();            // release pin
   OW_PIN_HIGH();            // enable pullup
   _delay_us(10);         // AN126 D
   sei();
   return 1;
}

///
///\brief read byte from 1-Wire bus
///
///\return byte read from bus
///
uint8_t ow_read_byte(void)
{
   uint8_t i, j;
   j = 0;
   for (i = 0; i < 8; i++)
   {
      j >>= 1;
      if (ow_read_bit())
         j |= 0x80;
   }

   return j;
}

///
///\brief write byte to 1-Wire bus
///
///\param data byte to write
///
uint8_t ow_write_byte(uint8_t data)
{
   uint8_t i;
   for (i = 0; i < 8; i++)
   {
      ow_write_bit(data & 0x01);
      data >>= 1;
   }
   return 1;
}

///
///\brief write match ROM command and ROM to bus
///
///\param rom ROM to write to bus
///
///\return 1 on success
///
uint8_t ow_match_rom(uint8_t *rom)
{
   int8_t i;
   if (ow_reset())
   {
      ow_write_byte(OW_COMMAND_MATCH_ROM);
      for (i = 7; i >= 0; i--)         // for each bit of rom code from LSB to MSB
         ow_write_byte(rom[i]);

      return 1;
   }
   return 0;
}

///
///\brief get CRC of given ROM
///
///\param rom ROM to calculate CRC of
///
///\return CRC of given ROM
///
uint8_t ow_check_crc(uint8_t *rom)
{
   int8_t i, crc = 0;

   for (i = 7; i >= 0; i--)
      crc = _crc_ibutton_update(crc, rom[i]);
   return crc;
}
