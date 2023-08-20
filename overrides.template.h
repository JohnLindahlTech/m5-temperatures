/** NOTE!
  If using overrides.h, make sure to override all values in here,
  even if going with the defaults, otherwise the project will not compile.
*/

//Upper Left
#define DEFAULT_NAME_0    "Temp 0"
#define TEMPERATURE_0     "m5/temperature/0"

//Upper Right
#define DEFAULT_NAME_1    "Temp 1"
#define TEMPERATURE_1     "m5/temperature/1"

//Lower Left
#define DEFAULT_NAME_2    "Temp 2"
#define TEMPERATURE_2     "m5/temperature/2"

//Lower Right
#define DEFAULT_NAME_3    "Temp 3"
#define TEMPERATURE_3     "m5/temperature/3"

#define REQUEST_UPDATE    "m5/request/update"
#define SLEEP             "m5/status/sleep"
#define WAKE              "m5/status/wake"

#define PIR_MOVEMENT      "m5/motion"
#define RHT_HUMIDITY      "m5/humidity"
#define RHT_TEMPERATURE   "m5/temperature"


#define ENABLE_PIR        false
#define PIR_PIN           34
#define PIR_TIMEOUT_SECONDS  5

#define QR_CODE "WIFI:S:Not Yet Configured;T:WPA;P:passwords are overrated;;";

#define RHT3_PIN 0