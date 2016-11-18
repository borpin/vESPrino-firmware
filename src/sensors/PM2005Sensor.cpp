#include <Wire.h>
#include "sensors/PM2005Sensor.hpp"
#include <LinkedList.h>
#include "interfaces/Pair.h"
#include "common.hpp"
#include "plugins/TimerManager.hpp"
#include <I2CHelper.hpp>

PM2005Sensor::PM2005Sensor() {
  registerSensor(this);
}

void dump(uint8_t *r, int len) {
  for (int i=0; i<len; i++) SERIAL_PORT << _HEX(*(r++)) << ",";
  SERIAL_PORT.println();
}

void PM2005Sensor::setup(MenuHandler *handler) {
  handler->registerCommand(new MenuEntry(F("pm2005quiet"), CMD_BEGIN, &PM2005Sensor::onCmdQuiet, F("pm2005quiet 22:00,03:00,180 (zulu start, zulu end, tz offset in minutes)")));
  handler->registerCommand(new MenuEntry(F("pm2005int"), CMD_BEGIN, &PM2005Sensor::onCmdInterval, F("pm2005int 20,3600 (measure time in seconds - active, quiet (<30sec = dynamic mode)")));
  if (I2CHelper::i2cSDA ==  -1) return ;
  if (I2CHelper::checkI2CDevice(0x28)) {
    Serial << F("Found PM2005 - Dust / Particle Sensor\n");
    hasSensor = true;
    checkMode();
  }
}

void PM2005Sensor::checkMode() {
  Wire.setClock(10000);
  int pm25, pm10, mode, status;
  if (!intReadData(pm25, pm10, status, mode)) return;
  String qs = PropertyList.readProperty(PROP_PM2005_QSTART);
  String qe = PropertyList.readProperty(PROP_PM2005_QEND);
  int interval = PropertyList.readLongProperty(PROP_PM2005_INT_ACT);
  if (!interval) interval = 360; // by defaul if property is missing, use timing mode each 6 minutes
  if (qs.length() > 0) {
    if (TimerManagerClass::isTimeInPeriod(qs.c_str(), qe.c_str()) != TP_OUT) {
      interval = PropertyList.readLongProperty(PROP_PM2005_INT_QUIET);
      if (DEBUG) Serial << F("PM2005: Quiet Time\n");
    } else {
      interval = PropertyList.readLongProperty(PROP_PM2005_INT_ACT);
      if (DEBUG) Serial << F("PM2005: Active Time\n");
    }
  }
  if (DEBUG) Serial << F("PM2005: Interval: ") << interval << F(", mode: ") << mode << endl;
  if (interval < 300 && mode != 5) setDynamicMode();
  else if (interval >= 300 && mode != interval) setTimingMeasuringMode(interval);
  //intReadData(pm25, pm10, status, mode);
}

void PM2005Sensor::onCmdQuiet(const char *line) {
  Wire.setClock(10000);
  char *s = strchr(line, ' ') + 1;
  char *e = strchr(s, ',');
  *e = 0;
  PropertyList.putProperty(PROP_PM2005_QSTART, s);
  s =  e + 1;
  e = strchr(s, ',');
  *e = 0;
  PropertyList.putProperty(PROP_PM2005_QEND, s);
  s =  e + 1;
  PropertyList.putProperty(PROP_TZOFFSET, s);
}

void PM2005Sensor::onCmdInterval(const char *line) {
  Wire.setClock(10000);
  char *s = strchr(line, ' ') + 1;
  char *e = strchr(s, ',');
  *e = 0;
  PropertyList.putProperty(PROP_PM2005_INT_ACT, s);
  s = e + 1;
  PropertyList.putProperty(PROP_PM2005_INT_QUIET, s);
}

void PM2005Sensor::getData(LinkedList<Pair *> *data) {
  if (!hasSensor) return;
  checkMode();
  int pm25, pm10, mode, status;
  if (!intReadData(pm25, pm10, status, mode)) return;
  data->add(new Pair("PM25", String(pm25)));
  data->add(new Pair("PM10", String(pm10)));
}

bool PM2005Sensor::intBegin() {
  Wire.setClock(10000);
  I2CHelper::i2cWireStatus();
  I2CHelper::i2cHigh();
  int res;
  for (int i=0; i < 5; i++) {
    Wire.beginTransmission(0x28);
    Wire.write(0x51);
    if (!(res = Wire.endTransmission(false))) {
      delay(20);
      return true;
    }
    delay(20);
    //Serial << "PM2005 init res: " << res << endl;
    I2CHelper::i2cHigh();
    delay(100);
  }
  return false;
}

void PM2005Sensor::setDynamicMode() {
  if (DEBUG) Serial << F("PM2005: set Dynamic Mode\n");
  uint8_t toSend[8] = {0x50, 0x16, 7, 5, 0, 0, 0, 0};
  sendCommand(toSend);
}

void PM2005Sensor::setTimingMeasuringMode(uint16_t intervalSec) {
  if (intervalSec > 60000) intervalSec = 60000;
  if (DEBUG) Serial << F("PM2005: set Timing Mode: ") << endl;
  uint8_t toSend[8] = {0x50, 0x16, 7, 4, (uint8_t)((intervalSec >> 8)&0xFF), (uint8_t)(intervalSec & 0xFF), 0, 0};
  sendCommand(toSend);
}

void PM2005Sensor::sendCommand(uint8_t *toSend) {
  Wire.setClock(10000);
  int res = -1;
  for (int i=1; i <=6; i++) toSend[7] ^= toSend[i];
  dump(toSend, 8);

  for (int i=0; i<5; i++) {
    Wire.beginTransmission(0x28);
    for (int k=0; k < 8; k++) Wire.write(toSend[k]);
    res = Wire.endTransmission();
    if (res == 0) break;
    delay(100);
  }
  if (DEBUG) Serial << F("PM2005 setMode res = ") << res << endl;
}

bool PM2005Sensor::intReadData(int &pm25, int &pm10, int &status, int &mode) {
  if (!intBegin()) {
    if (DEBUG) Serial << F("\nFailed to connect to PM2005\n");
    return false;
  }
  int r;
  byte data[22];
  byte cs = 0;
  r = Wire.requestFrom((uint8_t)0x28, (size_t)22, false);
  if (r != 22) {
    if (DEBUG) Serial << F("Expected 22 bytes, but got ") << r << endl;
    return false;
  }
  for (int i=0; i < 22; i++) {
    data[i] = Wire.read();
    if (DEBUG) Serial << _HEX(data[i]) << F(",");
    if (i < 21) cs ^= data[i];
  }
  if (DEBUG) Serial << endl;
  if (cs != data[21]) {
    if (DEBUG) Serial << F("Wrong Checksum: ") << cs << F(", expected: ") << data[21] << endl;
    return false;
  }
  status = data[2];
  pm25 = (data[5] << 8) + data[6];
  pm10 = (data[7] << 8) + data[8];
  mode = (data[9] << 8) + data[10];
  if (DEBUG) {
    Serial << F("Sensor Status: ") << status << endl;
    Serial << F("PM 2.5 : ") << pm25 << endl;
    Serial << F("PM  10 : ") << pm10 << endl;
    Serial << F("Measuring Mode : ") << mode << endl;
  }
  return true;
}
