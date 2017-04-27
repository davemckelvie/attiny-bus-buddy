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

#ifndef __DS18B20_H__
#define __DS18B20_H__

#define MAX_TEMP_SENSORS               3

//DS18x20 specific commands
#define DS_COMMAND_CONVERT             0x44
#define DS_COMMAND_READ_SP             0xBE
#define DS_COMMAND_READ_SCRATCHPAD     0xBE
#define DS_COMMAND_WRITE_SP            0x4E
#define DS_COMMAND_WRITE_SCRATCHPAD    0x4E
#define DS_COMMAND_COPY_SP             0x48
#define DS_COMMAND_COPY_SCRATCHPAD     0x48
#define DS_COMMAND_SAVE_SCRATCHPAD     0x48
#define DS_COMMAND_RECALL_EE           0xB8
#define DS_COMMAND_RECALL_EEPROM       0xB8
#define DS_COMMAND_READ_POWER          0xB4

//1-Wire commands
#define DS_COMMAND_SEARCH_ROM          0xF0
#define DS_COMMAND_READ_ROM            0x33
#define DS_COMMAND_SKIP_ROM            0xCC
#define DS_COMMAND_MATCH_ROM           0x55
#define DS_COMMAND_ALARM_SEARCH        0xEC

#define DS_FAMILY_CODE_INDEX           7
#define DS18B20_FAMILY_CODE            0x28

///\struct ds18b20_settings_t
///\brief container for ds18b20 settings
typedef struct {
   int8_t id;         ///< DS18B20 ID
   uint8_t rom[8];   ///< DS18B20 ROM
} ds18b20_settings_t;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t ds18b20_init(void);
int8_t ds18b20_read_temp(uint8_t);
uint8_t ds18b20_verify_crc(uint8_t);
#ifdef __cplusplus
}
#endif
#endif /*DS18B20_H_*/
