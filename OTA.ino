
void doHttpUpdate(int mode) {
  SERIAL << F("Starting Web update, mode ") << mode << endl;
  const __FlashStringHelper *prod, *test;
  if (deviceType == DT_VAIR) {
    prod = F("https://raw.githubusercontent.com/vlast3k/ESP8266_SERIALOTATS/master/fw/latest.bin");
    test = F("https://raw.githubusercontent.com/vlast3k/ESP8266_SERIALOTATS/master/fw/latest_test.bin");
  } else if (deviceType == DT_VTHING_CO2) {
    prod = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest.bin");
    test = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest_test.bin");
  } else if (deviceType == DT_VTHING_STARTER) {
    prod = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest_starter.bin");
    test = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest_starter_test.bin");
  } else if (deviceType == DT_VTHING_H801_LED) {
    prod = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest_h801.bin");
    test = F("https://raw.githubusercontent.com/vlast3k/vThingCO2/master/fw/latest.bin");
  }
  t_httpUpdate_return ret = ESPhttpUpdate.update(String(mode == 1 ? prod : test).c_str());
  switch(ret) {
    case HTTP_UPDATE_FAILED:
      SERIAL.println(F("HTTP_UPDATE_FAILED"));
      break;

    case HTTP_UPDATE_NO_UPDATES:
      SERIAL.println(F("HTTP_UPDATE_NO_UPDATES"));
      break;

    case HTTP_UPDATE_OK:
      SERIAL.println(F("HTTP_UPDATE_OK"));
      break;
  }
}

void handleOTA() {
  if (startedOTA) ArduinoOTA.handle();
}

void startOTA() {
  SERIAL << "Starting OTA..." << endl;
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    SERIAL.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    SERIAL.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    SERIAL.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    SERIAL.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) SERIAL.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) SERIAL.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) SERIAL.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) SERIAL.println("Receive Failed");
    else if (error == OTA_END_ERROR) SERIAL.println("End Failed");
  });

  ArduinoOTA.begin();
  SERIAL.println("Ready");
  startedOTA = true;
}

