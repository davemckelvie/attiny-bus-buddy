# Simple I2C Bus Buddy

This project turns an ATTiny85 into a simple I2C bus master/translator for interfacing the DS18B20 1-wire
temperature sensor with a host device via an I2C connection.

## It's an adapter

It's an adapter between an I2C host and 1-wire sensor.

## It's a facade

It simplifies the DS18B20 interface.

# How to use

This is a [PlatformIO](http://platformio.org/) project. Install PlatformIO then open this project for fun and profit.

## Hardware

This project uses a [Digispark](http://digistump.com/category/1). Any ATTiny85 with a [Micronucleus](https://github.com/micronucleus/micronucleus) should work.

## License

Copyright 2017  Dave McKelvie

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
