#pragma once
#if !defined(__ESPCONFIG__)
#define __ESPCONFIG__
#include <inttypes.h>
// #include <HardwareSerial.h>



struct serialPins {
    uint32_t TXPIN;
    uint32_t RXPIN;
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
#define ETH_CS (GPIO_NUM_5)//  spi cs pin  5
#define ETH_RST_PIN (GPIO_NUM_22) //hardware RESET should be held low at least 500 us for W5500


#define UARTtimeout 200 //time out of uart response in ms

static bool csUartTimeOut = false;
static bool cmUartTimeOut = false;



//wifi settings
const char* host = "ClockStation";
const char* ssid = "wifi";
const char* Wpass = "F31wv32P";


#if !( defined( ESP32 ) || defined(ESP8266) )
  #error This code is designed to run on ESP32 / ESP8266 platform! Please check your Tools->Board setting.
#endif

#define DEBUG_ETHERNET_GENERIC_PORT         Serial

// Debug Level from 0 to 4
#define _ETG_LOGLEVEL_                      2

// Default to use W5100. Must change to false for W5500, W5100S, for faster SPI clock
// #define USE_W5100                           false

#define ETHERNET_USE_ESP32
#define BOARD_TYPE      "ESP32"
#define W5500_RST_PORT   22

#include <SPI.h>

#include <soc/spi_pins.h>

#define USING_SPI2              false

#define PIN_MISO          MISO
#define PIN_MOSI          MOSI
#define PIN_SCK           SCK
#define PIN_SS            SS

#define SHIELD_TYPE       "W5x00 using Ethernet_Generic Library on SPI"



///////////////////////////////////////////////////////////

// W5100 chips can have up to 4 sockets.  W5200 & W5500 can have up to 8 sockets.
// Use EthernetLarge feature, Larger buffers, but reduced number of simultaneous connections/sockets (MAX_SOCK_NUM == 2)
#define ETHERNET_LARGE_BUFFERS

//////////////////////////////////////////////////////////

#include "Ethernet_Generic.h"

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

// Select the IP address according to your local network
IPAddress ip(192, 168, 2, 222);

// Google DNS Server IP
IPAddress myDns(8, 8, 8, 8);

#endif // __ESPCONFIG__
