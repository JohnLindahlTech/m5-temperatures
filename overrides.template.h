/** NOTE!
  If using overrides.h, make sure to override all values in here,
  even if going with the defaults, otherwise the project will not compile.
*/

//Upper Left
// Default / Legacy way to name the upper left view
#define DEFAULT_NAME_0    "Temp 0"
// MQTT Topic to subscribe to for upper left view
#define TEMPERATURE_0     "m5/temperature/0"

//Upper Right
// Default / Legacy way to name the upper right view
#define DEFAULT_NAME_1    "Temp 1"
// MQTT Topic to subscribe to for upper right view
#define TEMPERATURE_1     "m5/temperature/1"

//Lower Left
// Default / Legacy way to name the lower left view
#define DEFAULT_NAME_2    "Temp 2"
// MQTT Topic to subscribe to for lower left view
#define TEMPERATURE_2     "m5/temperature/2"

//Lower Right
// Default / Legacy way to name the lower right view
#define DEFAULT_NAME_3    "Temp 3"
// MQTT Topic to subscribe to for lower right view
#define TEMPERATURE_3     "m5/temperature/3"

// MQTT Topic to PUBLISH to request update of all statuses
#define REQUEST_UPDATE    "m5/request/update"
// MQTT Topic to SUBSCRIBE to request a sleep event
#define SLEEP             "m5/status/sleep"
// MQTT Topic to SUBSCRIBE to request a wake event
#define WAKE              "m5/status/wake"

// MQTT Topic to PUBLISH to send motion statuses ("motion", "no motion")
#define PIR_MOVEMENT      "m5/motion"
// MQTT Topic to PUBLISH to send RHT03 Humidity reading (Stringified float)
#define RHT_HUMIDITY      "m5/humidity"
// MQTT Topic to PUBLISH to send RHT03 Celsius temperature reading (Stringified float, compensated by RHT3_CELSIUS_CALIBRATION)
#define RHT_TEMPERATURE   "m5/temperature"

// Enable (or disable) local PIR sensor
#define ENABLE_PIR        false
// Which PIN PIR sensor is connected to
#define PIR_PIN           34
// Define in seconds how long the LCD should stay awake after a "no motion" event happens
#define PIR_TIMEOUT_SECONDS  5

// What is the message of the QR code
// If going for sharable WIFI Code, format:
// WIFI:S:ssid;T:WPA;P:password;;
// WIFI:
//      S:SSID of the network;
//             T:Security Protocol, probably in most modern scenarios WPA;   
//                   P: password of the network;;
#define QR_CODE "WIFI:S:Not Yet Configured;T:WPA;P:passwords are overrated;;"

// Enable (or disable) local RHT3 Temp & Humidity sensor
#define ENABLE_RHT3 false
// Which position to put the local RHT3 sensor on, (will not subscribe on MQTT for that position)
// 0 = Upper Left, 1 = Upper Right, 2 = Lower Left, 3 = Lower Right
#define RHT3_POSITION 3
// Which PIN RHT3 snesor is connected to
#define RHT3_PIN 0
// Offset of the temperature for the RHT sensor, in celsius. Minus will subtract from the sensor temp and plus will add.
#define RHT3_CELSIUS_CALIBRATION 0
