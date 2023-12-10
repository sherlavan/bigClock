#ifndef CS_READ_FUNCTIONS_H
#define CS_READ_FUNCTIONS_H
#include <ArduinoJson.h>
#include "uartClockStationCommands.h"
#include "Func.h"
#include "config.h"

void readBaseParameters(serialPins UARTDev, Stream & OutputStream);

#endif