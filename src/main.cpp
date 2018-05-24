/*
 * Copyright (C) 2017 David McKelvie.
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
#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include"onewire.h"
#include"ds18b20.h"

#define I2C_SLAVE_ADDRESS (0x66)
#include <TinyWireS.h>
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif
#define LED_PIN       (1)
#define ONE_WIRE_PIN  (4)
#define DELAY_TIME_MS (500)

// Pin port function
//   1 PB5  (can't be used for DS18B20)
//   2 PB3  micronucleus
//   3 PB4  micronucleus - DS18B20
//   4      GND
//   5 PB0  SDA
//   6 PB1  LED
//   7 PB2  SCL
//   8      VCC
static uint8_t current_device = 0;
static volatile uint8_t num_devices = 0;
static volatile uint8_t ds18b20_device_table[MAX_TEMP_SENSORS * RAW_DATA_SIZE] = {0};

/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the
 * send-buffer when using this callback
 */
void requestEvent()
{
  static volatile uint8_t index = 0;

  if (num_devices == 0) return;

  TinyWireS.send(ds18b20_device_table[index++]);
  if (index >= (num_devices * RAW_DATA_SIZE)) {
    index = 0;
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  num_devices = ds18b20_init();
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onRequest(requestEvent);
}

void loop()
{
  TinyWireS_stop_check();
  digitalWrite(LED_PIN, HIGH);
  tws_delay(DELAY_TIME_MS);

  if (num_devices > 0) {
    digitalWrite(LED_PIN, LOW);
    if (current_device >= num_devices) {
      current_device = 0;
    }
    ds18b20_read_temp_raw(current_device, ds18b20_device_table + (current_device * RAW_DATA_SIZE));
    current_device++;
    tws_delay(DELAY_TIME_MS);
  } else {
    num_devices = ds18b20_init();
  }
}
