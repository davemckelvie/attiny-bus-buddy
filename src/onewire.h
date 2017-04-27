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

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#define OW_PORT                     PORTB
#define OW_PINS                     PINB
#define OW_DDR                      DDRB
#define OW_PIN                      PB4

#define OW_DDR_OUT()                (OW_DDR |=  _BV(OW_PIN))
#define OW_DDR_IN()                 (OW_DDR &= ~_BV(OW_PIN))
#define OW_PIN_LOW()                (OW_PORT &= ~_BV(OW_PIN))
#define OW_PIN_HIGH()               (OW_PORT |=  _BV(OW_PIN))

#define OW_ASSERT()                 {OW_DDR_OUT();OW_PIN_LOW();}
#define OW_RELEASE()                {OW_DDR_IN();OW_PIN_HIGH();}

#define OW_COMMAND_SEARCH_ROM       0xF0
#define OW_COMMAND_READ_ROM         0x33
#define OW_COMMAND_SKIP_ROM         0xCC
#define OW_COMMAND_MATCH_ROM        0x55
#define OW_COMMAND_ALARM_SEARCH     0xEC

#ifdef __cplusplus
extern "C" {
#endif
uint8_t ow_reset(void);
uint8_t ow_read_bit(void);
uint8_t ow_write_bit(uint8_t);
uint8_t ow_read_byte(void);
uint8_t ow_write_byte(uint8_t);
uint8_t ow_search_rom(uint8_t *);
uint8_t ow_search_alarm(uint8_t *);
uint8_t ow_init_rom_search(void);
uint8_t ow_find_device(uint8_t *);
uint8_t ow_last_device_found(void);
uint8_t ow_match_rom(uint8_t *);
uint8_t ow_check_crc(uint8_t *);
#ifdef __cplusplus
}
#endif
#endif /*ONEWIRE_H_*/
