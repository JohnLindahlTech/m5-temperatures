// SSID of the network to connect to.
#define WIFI_SSID "" 
// Passphrase of the network to connect to.
#define WIFI_PASS ""

// MQTT Hostname/Domain/IP to connect to.
#define MQTT_HOST ""
// MQTT Port to connect to. Defaults: 1883 for MQTT, 8883 for MQTTS
#define MQTT_PORT 8883
// MQTT username to auth with.
#define MQTT_USER ""
// MQTT password to auth with.
#define MQTT_PASS ""
// MQTT ClientID, sort of like UserAgent. Must/should be unique.
#define MQTT_CLIENT_ID ""

// CA Root Cert for the MQTTS trust chain
#define CA_CERT "-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n"