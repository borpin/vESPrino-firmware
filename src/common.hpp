#ifndef common_h
#define common_h
#include "defines.h"
#include <CubicGasSensors.h>
#include <Streaming.h>
#include <NeoPixelBus.h>
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
#include "interfaces/Destination.hpp"
//#include "interfaces/Destination.hpp"
#include "interfaces/Plugin.hpp"
#include "interfaces/Sensor.hpp"
#include "destinations/CustomHTTPDest.hpp"
#include "destinations/SerialDumpDest.hpp"
#include "plugins/PropertyList.hpp"
//#include "plugins/TimerManager.hpp"
#include "plugins/PowerManager.hpp"
#include "sensors/SI7021Sensor.hpp"
#include "sensors/BME280Sensor.hpp"
#include "sensors/BMP085Sensor.hpp"
#include "sensors/PM2005Sensor.hpp"
#include "sensors/CDM7160Sensor.hpp"
#include "sensors/TestSensor.hpp"

#include "destinations/MQTTDest.hpp"
#include "destinations/RFDest.hpp"
#include <RTCMemStore.hpp>
#include <ESP8266WiFiMulti.h>
//#include "plugins/WifiStuff.hpp"
#include "plugins/NeopixelVE.hpp"
#include <I2CHelper.hpp>
#include "plugins/LoggingPrinter.hpp"


void OTA_registerCommands(MenuHandler *handler);
void MQTT_RegisterCommands(MenuHandler *handler);
void VESP_registerCommands(MenuHandler *handler);
void CO2_registerCommands(MenuHandler *handler);

void heap(const char * str);
void setSendInterval (const char *line);
void setSendThreshold(const char *line);
// void getTS(const char* line);
// void sendTS();
// void testUBI();
//void setWifi(const char* p);
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
//void handleWifi();
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
//void activeWait();
char *getJSONConfig(const char *item, char *buf, char *p1 = NULL, char *p3=NULL);
//void testJSON(const char *ignore);
// void testHttpUpdate();
void setSAPAuth(const char *);
char *extractStringFromQuotes(const char* src, char *dest, int destSize=19) ;
void cmdScanI2C(const char *ignore);
void printVersion(const char *ignore="");
void handleCommandVESPrino(char *line);
byte readLine(int timeout);
int handleCommand();
void sendPingPort(const char *p);
void oledHandleCommand(const char *cmd);
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

// RgbColor ledNextColor();
// void setLedColor(const RgbColor &color);
// void ledHandleColor(char *colorNew);
// void ledSetBrg(char *s);
// void ledHandleMode(char *cmd) ;


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
void dumpArray(const char *s);

char* getListItem(const char* str, char *buf, int idx, char sep=',');
int getListItemCount(const char* str);
void replaceDecimalSeparator(String &src);

//#endif
extern boolean hasSSD1306, hasSI7021, hasPN532, hasBMP180, hasBH1750, hasAPDS9960, hasPIR;
extern bool shouldSend;
extern bool DEBUG;
extern bool SLAVE;
extern bool SKIP_LOOP;
extern MenuHandler menuHandler;
extern String storedSensors;
extern Timer *tmrStopLED;
extern LinkedList<Plugin *> plugins;
extern LinkedList<Sensor *> sensors;
extern LinkedList<Destination *> destinations;
extern LinkedList<Thr *> thresholds;
//extern LinkedList<Timer *> timers ;
extern char commonBuffer200[200];
extern CustomHTTPDest customHTTPDest;
extern SerialDumpDest serialDumpDest;
extern PropertyListClass PropertyList;
//extern TimerManagerClass TimerManager;
extern PowerManagerClass PowerManager;
extern SI7021Sensor si7021Sensor;
extern BME280Sensor bme280Sensor;
extern BMP085Sensor bmp085Sensor;
extern PM2005Sensor pm2005Sensor;
extern CDM7160Sensor cdm7160Sensor;
extern TestSensor testSensor;
extern MQTTDest mqttDest;
extern bool deepSleepWake;
extern RTCMemStore rtcMemStore;
extern ESP8266WiFiMulti  *wifiMulti;
extern RFDest rfDest;
extern LoggingPrinter LOGGER;
//extern WifiStuffClass WifiStuff;
//extern NeopixelVE neopixel;

//void MigrateSettingsIfNeeded();
void onStopLED();
//void loop_IntThrHandler();
//void setup_IntThrHandler(MenuHandler *handler);

void registerDestination(Destination *destination);
void registerPlugin(Plugin *plugin);
void registerSensor(Sensor *sensor);
//wl_status_t waitForWifi(uint16_t timeoutMs = 15000);
//void wifiConnectMulti();
//void startAutoWifiConfig(const char *ch);
//void wifiOff();
void fireEvent(const char *name);
void initDecimalSeparator();
#endif
