#if !defined(__ESPCONFIG__)
#define __ESPCONFIG__
#include "HardwareSerial.h"

struct serialPins {
    gpio_num_t TXPIN;
    gpio_num_t RXPIN;
    unsigned long baud;
    uint32_t config; 

} UART_pins;

struct MobileInternet {
    const char *APN;
    const char *USR;
    const char *PAS;
} mobInet_settings;


#define TXD_PIN_ClockStation (GPIO_NUM_32) //GPIO_NUM_2
#define RXD_PIN_ClockStation (GPIO_NUM_33) //GPIO_NUM_4

#define RXD_PIN_ClockMeh (GPIO_NUM_13)//UART for rs485 connections to clock mehanics
#define RS485_PIN_ClockMeh (GPIO_NUM_12)//Enable pin for rs485 connections to clock mehanics !!hi at boot!!
#define TXD_PIN_ClockMeh (GPIO_NUM_14)//UART for rs485 connections to clock mehanics
#define RXD_PIN_Sim800 (GPIO_NUM_4)
#define TXD_PIN_Sim800 (GPIO_NUM_16)

// Pin# of the I²C clock signal for the Ethernet PHY
#define ETH_MDC_PIN     23

// Pin# of the I²C IO signal for the Ethernet PHY
#define ETH_MDIO_PIN    18


#define UARTtimeout 200 //time out of uart response in ms

static bool csUartTimeOut = false;
static bool cmUartTimeOut = false;


//wifi settings
const char* host = "ClockStation";
const char* ssid = "wifi";
const char* Wpass = "F31wv32P";

#endif // __ESPCONFIG__
