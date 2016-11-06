#include "plugins\GPIO.hpp"
#include "MenuHandler.hpp"
#include <Timer.h>



#define PROP_GPIOCFG F("gpio.cfg")

GPIOClass::GPIOClass() {
  registerPlugin(this);
}

void GPIOClass::setup(MenuHandler *handler) {
   handler->registerCommand(new MenuEntry(F("gpio_set"), CMD_BEGIN, GPIOClass::cmdGPIOSet, F("gpio_set <gpio> <1/0/F>")));
   String str = PropertyList.readProperty(PROP_GPIOCFG);   //,H4,L12,H1,P3  - P - for PIR - do a double check 100 ms after 1 seen
   initPortsFromString(str.c_str());
   oldState = getGPIOState();
}

void GPIOClass::cmdGPIOSet(const char *line) {
  char *s = strchr(line, ' ');
  int gpio;
  bool mode;
  if (!s) return;
  gpio = atoi(s+1);
  s = strchr(s+1, ' ');
  if (!s) return;
  if (s[1] == 'F') pinMode(gpio, INPUT);
  else {
    mode = (s[1] == '1');
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, mode);
  }
}

void GPIOClass::initPortsFromString(const char *s) {
  //,H4,L12,H1
  while ((s=strchr(s, ',')) != NULL) {
    s++;
    int gpio = atoi(s+1);
    setBit(ports, gpio, 1);
    setBit(modes, gpio, s[0] == 'H' || s[0] == 'P');
    setBit(doubleCheck, gpio, s[0] == 'P');
    pinMode(gpio, INPUT);
  }
}

void GPIOClass::setBit(uint32_t &val, int bit, int state) {
  if (state) val |= 1U << bit;
  else val &= ~(1U << bit);
}

bool GPIOClass::isBitSet(uint32_t val, int bit) {
  return ((val >> bit) &1);
}

uint32_t GPIOClass::getGPIOState() {
  uint32_t state = 0;
  for(int i=0; i < 32; i++) {
    if (isBitSet(ports, i)) {
      setBit(state, i, digitalRead(i));
    }
  }
  return state;
}

void GPIOClass::loop() {
  uint32_t currentState = getGPIOState();
  if (currentState == oldState) return;
  for (int i=0; i < 32; i++) {
    if (isBitSet(currentState, i) != isBitSet(oldState, i)) {
      //onGPIO_2_HIGH or onGPIO_12_LOW
      String cmd = "onGPIO_";
      cmd += i;
      if (isBitSet(currentState, i)) {
        if (isBitSet(doubleCheck, i)) {
          delay(100);
          if (digitalRead(i) == 0) {
            setBit(currentState, i, 0);
            continue;
          }
        }
        cmd += "_HIGH";
      }
      else cmd += "_LOW";
      menuHandler.scheduleCommand(cmd.c_str());
    }
  }
  oldState = currentState;
}