#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Streaming.h>
#include <EEPROM.h>
#include <Math.h>
#include <algorithm>    // std::min
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <SoftwareSerialESP.h>
#include <Si7021.h>
#include <ArduinoJson.h>


#include <Timer.h>
#include <RunningAverage.h>
#include <NeoPixelBus.h> 
extern "C" {
#include "user_interface.h"
}

void setSendInterval (const char *line);
void setSendThreshold(const char *line);
void getTS(const char* line);
void sendTS();
void testUBI();
int setWifi(const char* p);
void atCIPSTART(const char *p);
void mockATCommand(const char *line);
void cfgGENIOT(const char *p);
void cfgHCPIOT1(const char *p);
void cfgHCPIOT2(const char *p);
void sndIOT(const char *line);
void sndGENIOT(const char *line);
void sndHCPIOT(const char *line);
void addHCPIOTHeaders(HTTPClient *http, const char *token);
void sndSimple();
void configMQTT(const char *p);
void sendMQTT(String msg);
int processResponseCodeATFW(HTTPClient *http, int rc);
int CM1106_read();
void CM1106_init();
void doHttpUpdate(int mode);
void handleOTA();
void startOTA();
char *extractStringFromQuotes(const char* src, char *dest, int destSize);
void storeToEE(int address, const char *str, int maxLength);
void handleWifi();
void connectToWifi(const char *s1, const char *s2, const char *s3);
void wifiScanNetworks();
int wifiConnectToStoredSSID();
boolean processUserInput();
byte readLine(int timeout);
int handleCommand();
int sendPing();
int httpAuthSAP();
int checkSAPAuth();
void heap(const char * str);
void onTempRead();
void handleSAP_IOT_PushService();
void doSend();
void attachButton();
void processMessage(String payload);
void processCommand(String cmd);
void initLight();
void printJSONConfig();
void putJSONConfig(const char *key, const char *value);
void dumpTemp();
void factoryReset();
void testH801();

void h801_setup();
void h801_loop();
void h801_onConfigStored();
void h801_mqtt_connect();
void h801_processConfig(const char *p);

String getJSONConfig(const char *item);
void testJSON();

void setSAPAuth(const char *);
char *extractStringFromQuotes(const char* src, char *dest, int destSize=19) ;
#define EE_WIFI_SSID_30B 0
#define EE_WIFI_P1_30B 30
#define EE_WIFI_P2_30B 60
#define EE_IOT_HOST_60B 90
#define EE_IOT_PATH_140B 150
#define EE_IOT_TOKN_40B 290
#define EE_GENIOT_PATH_140B 330
#define EE_MQTT_SERVER_30B  470
#define EE_MQTT_PORT_4B     500
#define EE_MQTT_CLIENT_20B  504
#define EE_MQTT_USER_45B    524
#define EE_MQTT_PASS_15B    569
#define EE_MQTT_TOPIC_40B   584
#define EE_MQTT_VALUE_70B   624
#define EE_JSON_CFG_1000B   1000
//#define EE_LAST 694

#define DT_VTHING_CO2 1
#define DT_VAIR 2
#define DT_VTHING_STARTER 3
#define DT_VTHING_H801_LED 4

#define SAP_IOT_HOST "spHst"
#define SAP_IOT_DEVID "spDvId"
#define SAP_IOT_TOKEN "spTok"
#define SAP_IOT_BTN_MSGID "spBtMID"
#define H801_API_KEY "h801key"
#define XX_SND_INT  "xxSndInt"
#define XX_SND_THR  "xxSndThr"


int deviceType = DT_VAIR;
#define SERIAL Serial

//int deviceType = DT_VTHING_H801_LED
//#define SERIAL Serial1

String   mqttServer = "m20.cloudmqtt.com";
uint32_t mqttPort   = 19749;
String   mqttClient = "vAir_CO2_Monitor";
String   mqttUser   = "pndhubpk";
String   mqttPass   = "yfT7ax_KDrgG";
String   mqttTopic  = "co2Value";


String mmsHost = "iotmmsi024148trial.hanatrial.ondemand.com";
String deviceId = "e46304a8-a410-4979-82f6-ca3da7e43df9";
String authToken = "8f337a8e54bd352f28c2892743c94b3";
String colors[] = {"red","pink","lila","violet","blue","mblue","cyan","green","yellow","orange"};
#define COLOR_COUNT 10

//h iotmmsi024148trial.hanatrial.ondemand.com
//d c5c73d69-6a19-4c7d-9da3-b32198ba71f9
//m 2023a0e66f76d20f47d7
//v co2
//t 46de4fc404221b32054a8405f602fd

boolean DEBUG = false;

uint32_t intCO2RawRead   =  15000L;
uint32_t intCO2SendValue = 120000L;
uint16_t co2Threshold = 1;
uint32_t lastSentCO2value = 0;

Timer *tmrCO2RawRead, *tmrCO2SendValueTimer, *tmrTempRead, *tmrCheckPushMsg, *tmrStopLED;
boolean startedCO2Monitoring = false;
RunningAverage raCO2Raw(4);
NeoPixelBus *strip;// = NeoPixelBus(1, D4);
SI7021 *si7021;


bool shouldSend = false;

void sendCO2Value() {
  int val = (int)raCO2Raw.getAverage();
  if (val > 2000) SERIAL << "val is: " << val << endl;
  String s = String("sndiot ") + val;
  sndIOT(s.c_str());
  lastSentCO2value = val;
  tmrCO2SendValueTimer->Start();
}

void onCO2RawRead() {
  int res = CM1106_read();
  if (res != 550) startedCO2Monitoring = true;
  if (startedCO2Monitoring) {
    if (res > 2000) SERIAL << "res is: " << res << endl;
     SERIAL << "res is2: " << res << endl;
    raCO2Raw.addValue(res);
    int diff = raCO2Raw.getAverage() - lastSentCO2value;
    if ((co2Threshold > 0) && (abs(diff) > co2Threshold)) sendCO2Value();
  }
}

String VERSION = "v1.9";
void printVersion() {
  switch (deviceType) {
    case DT_VTHING_CO2:     SERIAL << endl << "vThing - CO2 Monitor "     << VERSION << endl; break;
    case DT_VAIR:           SERIAL << endl << "vAir - WiFi Module "       << VERSION << endl; break;
    case DT_VTHING_STARTER: SERIAL << endl << "vThing - Starter Edition " << VERSION << endl; break;
    case DT_VTHING_H801_LED: SERIAL << endl << "vThing - H801 Fw " << VERSION << endl; break;
  }  
}

void onStopLED() {
    strip->SetPixelColor(0, RgbColor(0, 0,0));
    strip->Show();      
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
//  SERIAL << "Start Setup: " << millis() << endl;
  printVersion();
  EEPROM.begin(3000);
  SERIAL << "ready" << endl;
  //SERIAL << "Strip begin: " << millis() << endl;
  //SERIAL << "Strip end: " << millis() << endl;
  //startWifi();
  SERIAL << "Waiting for auto-connect" << endl;
  delay(1000);
  if (deviceType == DT_VTHING_STARTER) {
    strip = new NeoPixelBus(1, D4);
    if (strip) {
      strip->Begin();
      strip->SetPixelColor(0, RgbColor(0, 5,0));
      strip->Show();  
    }    
    si7021 = new SI7021();
    si7021->begin(D1, D6); // Runs : Wire.begin() + reset()
    si7021->setHumidityRes(8); // Humidity = 12-bit / Temperature = 14-bit
    dumpTemp();
    tmrTempRead = new Timer(15000L,    onTempRead);
    tmrCheckPushMsg = new Timer(1000L, handleSAP_IOT_PushService);
    tmrTempRead->Start();
    tmrCheckPushMsg->Start();
    attachButton();
  } else if (deviceType == DT_VTHING_CO2 ) {
    strip = new NeoPixelBus(1, D4);
    if (strip) {
      strip->Begin();
      strip->SetPixelColor(0, RgbColor(0, 5,0));
      strip->Show();  
    }
    if (getJSONConfig(XX_SND_INT)) intCO2SendValue = getJSONConfig(XX_SND_INT).toInt();
    if (getJSONConfig(XX_SND_THR)) co2Threshold    = getJSONConfig(XX_SND_THR).toInt();
    
    tmrStopLED           = new Timer(10000, onStopLED, true);
    tmrCO2RawRead        = new Timer(intCO2RawRead,   onCO2RawRead);
    tmrCO2SendValueTimer = new Timer(intCO2SendValue, sendCO2Value);
    int res = CM1106_read();
    SERIAL << "CO2 now: " << res << endl;
    tmrCO2RawRead->Start();
    tmrStopLED->Start();
  } else if (deviceType == DT_VTHING_H801_LED) {    
    h801_setup();
  }
  //SERIAL << "Completed Setup: " << millis() << endl;
//WiFi.mode(WIFI_OFF);
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
    //WiFi.begin("vladiHome", "0888414447");
  
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    SERIAL.print(".");
//  }
}


boolean startedOTA = false;
void loop() {
  handleWifi();
  handleOTA();
  if (!startedOTA) {
    while (processUserInput()) delay(1000);
  }

  if (deviceType == DT_VTHING_CO2) {
    tmrCO2RawRead->Update();
    tmrCO2SendValueTimer->Update();
    tmrStopLED->Update();
    delay(5000);
  } else if (deviceType == DT_VTHING_STARTER) {
    tmrTempRead->Update();
    //SERIAL << "\n\n\n------ before push service\n\n\n";
    tmrCheckPushMsg->Update();
    //handleSAP_IOT_PushService();
    //SERIAL << "\n\n\n------ before do Send\n\n\n";
    doSend();
    //SERIAL << "\n\n\n------ before delay 5 sec\n\n\n";
    //SERIAL << ".";
    delay(1000);
  } else if (deviceType == DT_VTHING_H801_LED) {
    h801_loop();
//    Serial1 << "serial1" << endl;
//    Serial <<"serial" << endl;
//    SERIAL << "SERIAL" << endl;
//    delay(500);
  }
}
