#include <M5Core2.h>

#include <ssl_client.h>
#include <WiFiClientSecure.h>

#include <PubSubClient.h>

#include <WiFi.h>
#include "time.h"
#include "rht03.h"

#include "credentials.h"

#if defined __has_include
#if __has_include("overrides.h")
#include "overrides.h"
#else

// Upper Left
#define DEFAULT_NAME_0    "Loading"
#define TEMPERATURE_0     "m5/temperature/0"

//Upper Right
#define DEFAULT_NAME_1    "Loading"
#define TEMPERATURE_1     "m5/temperature/1"

//Lower Left
#define DEFAULT_NAME_2    "Loading"
#define TEMPERATURE_2     "m5/temperature/2"

//Lower Right
#define DEFAULT_NAME_3    "Loading"
#define TEMPERATURE_3     "m5/temperature/3"

#define REQUEST_UPDATE    "m5/request/update"
#define SLEEP             "m5/status/sleep"
#define WAKE              "m5/status/wake"

#define PIR_MOVEMENT      "m5/motion"
#define RHT_HUMIDITY      "m5/humidity"
#define RHT_TEMPERATURE      "m5/temperature"

// PIR Sensor
#define ENABLE_PIR        false
#define PIR_PIN           19
#define PIR_TIMEOUT_SECONDS  5

#define QR_CODE "WIFI:S:Not Yet Configured;T:WPA;P:passwords are overrated;;";

#define TEMP_PIN 0

#endif
#endif

WiFiClientSecure wifiClientSecure;
PubSubClient client(wifiClientSecure);

// Remember: Make sure to make a `credentials.h` based on `credentials.template.h` 
const char* mqttServer = MQTT_HOST;
const int   mqttPort = MQTT_PORT;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASS;
const char* mqttClientId = MQTT_CLIENT_ID;
const char* ca_cert = CA_CERT;
const char* ssid       = WIFI_SSID;
const char* password   = WIFI_PASS;

long wifiLastReconnectAttempt = 0;
long mqttLastReconnectAttempt = 0;
long lastReceivedMessage = 0;
long lastReceivedTimestamp = 0;

char payloadDelimiter = '|';

// MQTT
char name0[50] = DEFAULT_NAME_0;
char temperature0[50] = "";
char name1[50] = DEFAULT_NAME_1;
char temperature1[50] = "";
char name2[50] = DEFAULT_NAME_2;
char temperature2[50] = "";
char name3[50] = DEFAULT_NAME_3;
char temperature3[50] = "";

// PIR
bool pir_movement = false;
bool pir_reported = false;
long pir_no_motion_timestamp = 0;
long pir_motion_timestamp = 0;

// WIFI QR
const char* qrCode = QR_CODE;
bool qrCodeVisible = false;
bool lcdAwake = true;

// Humidity and Temp
hw_timer_t *rht_timer = NULL;
RHT03 rht;
bool rht_reported = false;
bool rht_updated = false;
float rht_humidity = 0.0;
float rht_temperature = 0.0;

char rht_tmp[50] = "";
char rht_hmd[50] = "";


// 320 x 240

const int width = 320; // 320
const int height = 240; // 240
const int padding = 5;
const int lineHeight = 30;
const int radius = 10;

void wakeLcd(){
  lcdAwake = true;
  M5.Lcd.wakeup();
}
void sleepLcd(){
  lcdAwake = false;
  M5.Lcd.sleep();
}

void printDot(uint32_t color){
  M5.Lcd.fillCircle(160, 120, 5, color);
}

void printBase(){
  printUpperLeft(name0, "-", GREEN);
  printUpperRight(name1, "-", GREEN);
  printLowerLeft(name2, "-", GREEN);
  printLowerRight(name3, "-", GREEN);
}

void printUpperLeft(char* txt, char* temperature, uint32_t color){
  M5.Lcd.setTextColor(color);
  M5.Lcd.fillRoundRect(5, 5, 150, 110, radius, BLACK);
  M5.Lcd.drawRoundRect(5, 5, 150, 110, radius, color);
  M5.Lcd.setTextPadding(width / 2);
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString(txt, 80, 20, 1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString(temperature, 80, 60, 1);
}

void printUpperRight(char* txt, char* temperature, uint32_t color){
  M5.Lcd.setTextColor(color);
  M5.Lcd.fillRoundRect(width / 2 + padding, padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, BLACK);
  M5.Lcd.drawRoundRect(width / 2 + padding, padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, color);
  M5.Lcd.setTextPadding(width / 2);
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString(txt, 240, 20, 1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString(temperature, 240, 60, 1);
}

void printLowerLeft(char* txt, char* temperature, uint32_t color){
  M5.Lcd.setTextColor(color);
  M5.Lcd.fillRoundRect(padding, height / 2 + padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, BLACK);
  M5.Lcd.drawRoundRect(padding, height / 2 + padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, color);
  M5.Lcd.setTextPadding(width / 2);
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString(txt, 80, 140, 1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString(temperature, 80, 180, 1);
}

void printLowerRight(char* txt, char* temperature, uint32_t color){
  M5.Lcd.setTextColor(color);
  M5.Lcd.fillRoundRect(width / 2 + padding, height / 2 + padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, BLACK);
  M5.Lcd.drawRoundRect(width / 2 + padding, height / 2 + padding, width / 2 - 2*padding, height / 2 - 2*padding, radius, color);
  M5.Lcd.setTextPadding(M5.Lcd.width() / 2);
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString(txt, 240, 140, 1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString(temperature, 240, 180, 1);
}

uint32_t getWifiErrorColor(int type){
  switch(type){
    case WL_IDLE_STATUS: return MAROON;
    case WL_CONNECT_FAILED: return PINK;
    case WL_CONNECTION_LOST: return DARKGREY;
    case WL_DISCONNECTED: return WHITE;
    case 1000: return RED;
    case 1100: return GREEN;
  }
}

void printWifiError(boolean active, int errorType){
  uint32_t color = getWifiErrorColor(errorType);
  if(active){
    printDot(color);
  } else {
    printDot(BLACK);
  }
}

void printMQTTError(boolean active){
  if(active){
    printDot(ORANGE);
  } else {
    printDot(BLACK);
  }
}

void printMQTTReceived(boolean active){
    if(active){
    printDot(GREEN);
  } else {
    printDot(BLACK);
  }
}

void print0(){
  uint32_t color = getTemperatureColor(temperature0);
  printUpperLeft(name0, temperature0, color);
}

void print1(){
  uint32_t color = getTemperatureColor(temperature1);
  printUpperRight(name1, temperature1, color);
}

void print2(){
  uint32_t color = getTemperatureColor(temperature2);
  printLowerLeft(name2, temperature2, color);
}

void print3(){
  uint32_t color = getTemperatureColor(temperature3);
  printLowerRight(name3, temperature3, color);
}

void ensureWifi(const char* network, const char* pw, boolean forceBegin){
  long now = millis();
  if(forceBegin){
    printWifiError(true, 1000);
    WiFi.disconnect();
    WiFi.setAutoReconnect(true);
    WiFi.begin(network, pw);
    wifiLastReconnectAttempt = millis();
  }
  int currentStatus = WiFi.status();
  if(currentStatus == WL_CONNECTED && wifiLastReconnectAttempt > 0){
    printWifiError(false, 1100);
    wifiLastReconnectAttempt = 0;
    // We are connected, lets do nothing.
    return;
  }
  if(now - wifiLastReconnectAttempt > 5000){
 
    if(currentStatus == WL_IDLE_STATUS){
      
      if(now - wifiLastReconnectAttempt > 15000){
          printWifiError(true, 1000);
          WiFi.disconnect();
          WiFi.begin(network, pw);
          wifiLastReconnectAttempt = millis();
      } else {
        printWifiError(true, currentStatus);
      }
      return;
    }
    if(currentStatus == WL_CONNECT_FAILED || currentStatus == WL_CONNECTION_LOST || currentStatus == WL_DISCONNECTED){
      printWifiError(true, currentStatus);
      WiFi.disconnect();
      WiFi.begin(network, pw);
      wifiLastReconnectAttempt = millis();
      return;
    }
  }

}

void setupMQTT(){
  wifiClientSecure.setCACert(ca_cert);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(receivedCallback);
}

void updateVar(char var[], char buf[]){
  memset(var, '\0', sizeof(var));
  strcpy(var, buf);
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  int foundDelimiter = -1;
  char buf[50] = "\0";
  char name[50] = "\0";
  char temp[50] = "\0";
  memset(name, '\0', strlen(name));
  memset(temp, '\0', strlen(temp));
  for (int i = 0; i < length; i++) {
    if(((char)payload[i]) == payloadDelimiter){
      foundDelimiter = i;
    } else if(foundDelimiter >= 0){
      name[i - (foundDelimiter + 1)] = ((char)payload[i]);
    } else {
      temp[i] = ((char)payload[i]);
    }
  }

  if(strcmp(topic,TEMPERATURE_0) == 0){
    updateVar(temperature0, temp);
    if(foundDelimiter >= 0){
      updateVar(name0, name);
    }
  } else if(strcmp(topic,TEMPERATURE_1) == 0){
    updateVar(temperature1, temp);
    if(foundDelimiter >= 0){
      updateVar(name1, name);
    }
  } else if(strcmp(topic, TEMPERATURE_2) == 0){
    updateVar(temperature2, temp);
    if(foundDelimiter >= 0){
      updateVar(name2, name);
    }

  } else if(strcmp(topic,TEMPERATURE_3) == 0){
    updateVar(temperature3, temp);
    if(foundDelimiter >= 0){
      updateVar(name3, name);
    }

  } else if(strcmp(topic, WAKE) == 0){
    wakeLcd();
  } else if(strcmp(topic, SLEEP) == 0){
    sleepLcd();
  }
  printAll();
  printMQTTReceived(true);
  lastReceivedMessage = millis();
  lastReceivedTimestamp = millis();
}

void printAll(){
  if(!qrCodeVisible){
    print0();
    print1();
    print2();
    print3();
  }
}

uint32_t getTemperatureColor(char * temp){
  double num = atof(temp);
  if(num >= 25){
    return RED; // RED OK
  } else if (num >= 20){
    return ORANGE; // ORANGE OK
  } else if (num >= 15){
    return GREENYELLOW; // DARKGREEN BAD
  } else if (num >= 10){
    return GREEN; //GREEN; OK
  } else if (num >= 5){
    return CYAN; //CYAN; OK
  } else if(num >= 0){
    return LIGHTGREY;
  }
  return WHITE; //WHITE; TBD
}


boolean mqttConnect(boolean silent) {
  /* Loop until reconnected */
  if (!client.connected()) {
    if(!silent){
      M5.Lcd.print("MQTT connecting ...");
    } else {
      printMQTTError(true);
    }

    /* client ID */

    /* connect now */
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      if(!silent){
        M5.Lcd.println("connected");
      } else {
        printMQTTError(false);
      }
      /* subscribe topic */
      // client.subscribe(WAKE); // TODO
      // client.subscribe(SLEEP); // TODO
      client.subscribe(TEMPERATURE_2);
      // client.subscribe(TEMPERATURE_3);
      client.subscribe(TEMPERATURE_0);
      client.subscribe(TEMPERATURE_1);
      client.publish(REQUEST_UPDATE, "true", false);
    } else {
      if(!silent){
        M5.Lcd.print("failed, status code =");
        M5.Lcd.print(client.state());
        M5.Lcd.println("try again in 5 seconds");
      } else {
        printMQTTError(true);
      }
      /* Wait 5 seconds before retrying */
      
    }
  }
  return client.connected();
}

void detectedMovement(){
  wakeLcd();
  pir_movement = true;
  pir_reported = false;
  pir_motion_timestamp = millis();
}

void detectedNoMovement(){
  pir_movement = false;
  pir_reported = false;
  pir_no_motion_timestamp = millis();
}

void IRAM_ATTR detectsMovement() {
  int pirState = digitalRead(PIR_PIN);
  if(pirState == LOW){
    detectedNoMovement();
  } else if (pirState == HIGH) {
    detectedMovement();
  }
}

bool timerTrigger = true;
void IRAM_ATTR onRHTTimer(){
  timerTrigger = true;
}


void c0(void *pvparameters){
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectsMovement, CHANGE);
  rht_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(rht_timer, &onRHTTimer, true);
  timerAlarmWrite(rht_timer, 15 * 1000000, true);
  timerAlarmEnable(rht_timer); //Just Enable
  
  for(;;){
    delay(10);
    if(!pir_movement){
      long now = millis();

      if(now > pir_no_motion_timestamp + (PIR_TIMEOUT_SECONDS * 1000)){
        sleepLcd();
      }
    }
    if(timerTrigger){
      timerTrigger = false;
      int updateRet = rht.update();
      if (updateRet == 1){
        rht_updated = true;
        rht_reported = false;
        rht_humidity = rht.humidity();
        rht_temperature = rht.tempC();
        dtostrf(rht_temperature, 6, 1, rht_tmp);
        dtostrf(rht_humidity, 6, 1, rht_hmd);
        updateVar(temperature3, rht_tmp);
      } else {
        Serial.println("Error: " + String(updateRet));
      }
    }
  }
}



void setup(){
  Serial.begin(9600);
  M5.begin(true, false, true, false);
  rht.begin(RHT3_PIN);
  M5.Axp.SetLed(0);
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextSize(1);

  if(ENABLE_PIR){
    xTaskCreatePinnedToCore(c0, "Core0 Loop", 2000, NULL, 0, NULL, 0);
  }
  

  printBase();
  setupMQTT();
  ensureWifi(ssid, password, true);
}

void printQr(){
  qrCodeVisible = true;
  M5.Lcd.clear();
  M5.Lcd.qrcode(qrCode, 40, 0, 240, 4);

}

void hideQr(){
  qrCodeVisible = false;
  M5.Lcd.clear();
  printAll();
}

void buttonAPress(){
  wakeLcd();
  if(qrCodeVisible){
    hideQr();
  } else {
    printQr();
  }
}

void buttonBPress(){
  wakeLcd();
  client.publish(REQUEST_UPDATE, "true", false);
  timerTrigger = true;
}

void buttonCPress(){
  if(lcdAwake){
    sleepLcd();
  }else {
    wakeLcd();
  }
}

void loop(){
  delay(1);
  
  M5.update();
  if (M5.BtnA.wasPressed()) {
    buttonAPress(); 
  }

  if (M5.BtnC.wasPressed()) {
    buttonCPress();
  }

  if(M5.BtnB.wasPressed()){
    buttonBPress();
  }

  if(!pir_reported){
    pir_reported = true;
    client.publish(PIR_MOVEMENT, pir_movement ? "motion" : "no motion", false);
  }
  if(!rht_reported && rht_updated){
    rht_reported = true;
    print3();
    client.publish(RHT_HUMIDITY, rht_hmd, false);
    client.publish(RHT_TEMPERATURE, rht_tmp, false);
  }
  ensureWifi(ssid, password, false);
  long now = millis();
  if(WiFi.status() == WL_CONNECTED && !client.connected()){
    printMQTTError(true);
    
    if(now - mqttLastReconnectAttempt > 5000){
      mqttLastReconnectAttempt = now;
      if(mqttConnect(true)){
        printMQTTError(false);
        mqttLastReconnectAttempt = 0;
      }
    }

  } else {
    client.loop();
  }
  if(lastReceivedTimestamp > 0 && now - lastReceivedTimestamp > 1000 * 60 * 60){ // 1hour old data
     printBase();
     sleepLcd();
     lastReceivedTimestamp = 0;
  }
  if(lastReceivedMessage > 0 && now - lastReceivedMessage > 500){
    printMQTTReceived(false);
    lastReceivedMessage = 0;
  }

}