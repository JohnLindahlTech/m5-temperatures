# m5-temperatures
 M5Stack Core2 Temperature Display

## Hardware
M5Stack Core 2 ESP32 IoT Dev kit

[https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit](https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit)

## Setup
### Arduino IDE
[https://www.arduino.cc/en/software](https://www.arduino.cc/en/software) 

### Dependencies
* Libraries & Drivers for Core 2: [https://docs.m5stack.com/en/quick_start/arduino](https://docs.m5stack.com/en/quick_start/arduino)

### credentials.h
To be able to compile the project you must provide a `credentials.h` file, based on [credentials.template.h](./credentials.template.h)


## MQTT protocol
The project depends on a certain MQTT structure for topics and message-types.

### Overrides in overrides.h
If the below defaults does not fit your need, you can override the definitons in a files called `overrides.h`, based on [overrides.template.h](./overrides.template.h).
This is strongly recommended to utilize if needing to use `Legacy mode` of the temperature protocol, as you will have to hardcode names of indicators.


### `m5/temperature/[0-3]`
Subscription: The payload of the temperature supports 2 modes of operation.
* Legacy mode: `signed number` - Just send the temperature as a signed floating point (or `-` for no value). Examples: `-3.5`, `0.0`, `1.0`, `-`
* Dynamic mode: `signed number|name` - Pipe separated temperature as a signed floating point (or `-` for no value) and the name of the temperature to display. Examples: `-3.5|Kitchen`, `0.0|Bedroom`, `1.0|Entry`, `-|Backside`

### `m5/request/update`
Publish: The M5 will send a request to update at certain times (i.e. when starting up and button B pressed), with the payload `true`. Your server should then publish updates on the `m5/temperature/[0-3]` topics.

### `m5/status/sleep`
Subscription: When a message is received on this topic, the screen will go to sleep, updates will continue in the background, this is to save the display, but does not conserve battery.

### `m5/status/wake`
Subscription: When a message is received on this topic, the screen will wake, and present current values. (Recommendation, make sure that your server send updates on the `m5/temperature/[0-3]` topics at the same time).

### `m5/pir/motion`
Publish: Will publish updates to when the PIR sensors identifies motion or not. `motion` / `no motion`


## RHT03 Temp/Humidity sensor
See [https://github.com/sparkfun/SparkFun_RHT03_Arduino_Library](https://github.com/sparkfun/SparkFun_RHT03_Arduino_Library)

Install Library: `SparkFun RHT03 Arduino Library`

## Roadmap
* Dynamic count of temperatures.
  * Scale up UI if less than 4 temperatures on screen.
  * Pagination/Carousel if many temperatures.
  * Touch Screen Main Menu to Scale/"Zoom" UI. (e.g. Show 1, 4, 16 temperatures on screen).


## Troubleshoot

### Burn program on MacOS
To program the Core 2 from Mac, remember to pull `G0` (4th pin from bottom, on right side) low, by grounding (top 3 pins on left side), before (and during start of) the uploading phase.

Note/unsure: Should probably not be `G0`, but rather `RST`/`EN`, which is pin 3 from top on right side.
