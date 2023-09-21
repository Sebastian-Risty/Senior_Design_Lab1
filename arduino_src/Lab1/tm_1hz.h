#ifndef tm_1hz_h
#define tm_1hz_h

#include "sw_global.h"
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void tm_1hz();

#endif