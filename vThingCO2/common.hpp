#ifndef common_h
#define common_h

#include <NeoPixelBus.h>
#include <CubicGasSensors.h>
#include <Streaming.h>
//#include <EEPROM.h>
#include <Math.h>
#include <algorithm>    // std::min
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Timer.h>
#include "MenuHandler.hpp"
extern "C" {
#include "user_interface.h"
}
#ifdef ESP8266
  #include <SoftwareSerialESP.h>
#else
  #include <SoftwareSerial.h>
#endif
#include "LinkedList.h"
#include "interfaces\Destination.hpp"
#include "interfaces\Plugin.hpp"
#include "interfaces\Sensor.hpp"
#include "destinations\CustomHTTPDest.hpp"
#include "plugins\PropertyList.hpp"

#define Cred     RgbColor(255, 0, 0)
#define Cpink    RgbColor(255, 0, 128)
#define Clila    RgbColor(255, 0, 255)
#define Cviolet  RgbColor(128, 0, 255)
#define Cblue    RgbColor(0, 0, 255)
#define Cmblue   RgbColor(0, 128, 255)
#define Ccyan    RgbColor(0, 255, 255)
#define Cgreen   RgbColor(0, 255, 0)
#define Cyellow  RgbColor(255, 255, 0)
#define Corange  RgbColor(255, 100, 0)
#define Cwhite   RgbColor(255, 255, 255)
#define Cblack   RgbColor(0)

#define SERIAL Serial
#define EE_WIFI_SSID F("wifi.ssid")
#define EE_WIFI_P1   F("wifi.p1")
#define EE_WIFI_P2   F("wifi.p1")
#define EE_IOT_HOST  F("sap.hcp.host")
#define EE_IOT_PATH  F("sap.hcp.path")
#define EE_IOT_TOKN  F("sap.hcp.token")
#define EE_GENIOT_PATH  F("custom_url.path")
#define EE_MQTT_SERVER  F("mqtt.server")
#define EE_MQTT_PORT    F("mqtt.port")
#define EE_MQTT_CLIENT  F("mqtt.client")
#define EE_MQTT_USER    F("mqtt.user")
#define EE_MQTT_PASS    F("mqtt.pass")
#define EE_MQTT_TOPIC   F("mqtt.topic")
#define EE_MQTT_VALUE   F("mqtt.valie")
#define EE_RESET_CO2_1B 1


#define PROP_SND_INT      F("cubic.co2.interval")
#define PROP_SND_THR      F("cubic.co2.threshold")
#define PROP_H801_API_KEY F("h801.api_key")
// #define EE_WIFI_SSID_30B 0
// #define EE_WIFI_P1_30B 30
// #define EE_WIFI_P2_30B 60
#define EE_IOT_HOST_60B 90
#define EE_IOT_PATH_140B 150
#define EE_IOT_TOKN_40B 290
// #define EE_GENIOT_PATH_140B 330
// #define EE_MQTT_SERVER_30B  470
// #define EE_MQTT_PORT_4B     500
// #define EE_MQTT_CLIENT_20B  504
// #define EE_MQTT_USER_45B    524
// #define EE_MQTT_PASS_15B    569
// #define EE_MQTT_TOPIC_40B   584
// #define EE_MQTT_VALUE_70B   624
// #define EE_1B_RESET_CO2     694
// //#define EE_LAST 695
#define EE_JSON_CFG_1000B   1000

#define SAP_IOT_HOST "spHst"
#define SAP_IOT_DEVID "spDvId"
#define SAP_IOT_TOKEN "spTok"
#define SAP_IOT_BTN_MSGID "spBtMID"
//#define H801_API_KEY "h801key"
//#define XX_SND_INT  "xxSndInt"
//#define XX_SND_THR  "xxSndThr"

#define HTTP_STR  "http://"
#define HTTPS_STR "https://"

void OTA_registerCommands(MenuHandler *handler);
void WIFI_registerCommands(MenuHandler *handler);
void MQTT_RegisterCommands(MenuHandler *handler);
void VESP_registerCommands(MenuHandler *handler);
void CO2_registerCommands(MenuHandler *handler);

void heap(const char * str);
void setSendInterval (const char *line);
void setSendThreshold(const char *line);
// void getTS(const char* line);
// void sendTS();
// void testUBI();
void setWifi(const char* p);
// void atCIPSTART(const char *p);
// void mockATCommand(const char *line);
// void cfgGENIOT(const char *p);

void sndIOT(const char *line);
// void sndGENIOT(const char *line);
// void sndSimple();
void configMQTT(const char *p);
void sendMQTT(String msg);
// int processResponseCodeATFW(HTTPClient *http, int rc);
char *extractStringFromQuotes(const char* src, char *dest, int destSize);
void storeToEE(int address, const char *str, int maxLength);
void handleWifi();
// void connectToWifi(const char *s1, const char *s2, const char *s3);
// void wifiScanNetworks();
// int wifiConnectToStoredSSID();
//boolean processUserInput();
//byte readLine(int timeout);
//int handleCommand();
void heap(const char * str);
void processMessage(String payload);
void processCommand(String cmd);
void initLight();
void printJSONConfig(const char *ignore);
void putJSONConfig(const char *key, int value, boolean commit = true);
void putJSONConfig(const char *key, const char *value, boolean isArrayValue = false, boolean commit = true);
void dumpTemp();
void factoryReset(char *line = NULL);
void activeWait();
char *getJSONConfig(const char *item, char *buf, char *p1 = NULL, char *p3=NULL);
//void testJSON(const char *ignore);
// void testHttpUpdate();
void setSAPAuth(const char *);
char *extractStringFromQuotes(const char* src, char *dest, int destSize=19) ;
void scani2c(const char *ignore);
void printVersion(const char *ignore="");
void handleCommandVESPrino(char *line);
byte readLine(int timeout);
int handleCommand();
void sendPingPort(const char *p);
void oledHandleCommand(char *cmd);
void loopNeoPixel();
void checkButtonSend();

//#endif

//#ifdef VTHING_STARTER
boolean si7021init();
void onTempRead();
void doSend();
void attachButton();
void initVThingStarter();
void loopVThingStarter();

void handleCommandVESPrino(char *line);

void initPIR();
void handlePIR();

void initPN532();
void checkForNFCCart();

void initAPDS9960();
void handleGesture();
void checkForGesture();

boolean si7021init();
void onTempRead();
void dumpTemp();

boolean getDweetCommand(char *cmd);
void onGetDweets();
void handleDWCommand(char *line);

RgbColor ledNextColor();
void setLedColor(const RgbColor &color);
void ledHandleColor(char *colorNew);
void ledSetBrg(char *s);
void ledHandleMode(char *cmd) ;

boolean checkI2CDevice(int sda, int sca, int addr) ;


//#endif

#ifdef VTHING_H801_LED
void stopH801();
void testH801();
void h801_setup();
void h801_loop();
void h801_onConfigStored();
void h801_mqtt_connect();
void h801_processConfig(const char *p);
#endif
//#ifdef VTHING_CO2

void initCO2Handler();
void loopCO2Handler();


//#endif
extern boolean hasSSD1306, hasSI7021, hasPN532, hasBMP180, hasBH1750, hasAPDS9960, hasPIR;
extern bool shouldSend;
extern boolean DEBUG;
extern boolean SKIP_LOOP;
extern MenuHandler menuHandler;
extern Timer *tmrStopLED;
extern NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod>  *strip;// = NeoPixelBus(1, D4);
extern LinkedList<Plugin *> plugins;
extern LinkedList<Sensor *> sensors;
extern LinkedList<Destination *> destinations;
extern LinkedList<Thr *> thresholds;
extern LinkedList<Timer *> timers ;
extern char commonBuffer200[200];
extern CustomHTTPDest customHTTPDest;
extern PropertyListClass PropertyList;

#endif

void MigrateSettingsIfNeeded();
void onStopLED();
void loop_IntThrHandler();
void setup_IntThrHandler(MenuHandler *handler);

void registerDestination(Destination *destination);
void registerPlugin(Plugin *plugin);
