#if !defined(__ESPCONFIG__)
#define __ESPCONFIG__

#define TXD_PIN_ClockStation (GPIO_NUM_2)//(GPIO_NUM_5) for esp32c3
#define RXD_PIN_ClockStation (GPIO_NUM_4)//(GPIO_NUM_4) for esp32c3

#define RXD_PIN_ClockMeh (GPIO_NUM_35)//UART for rs485 connections to clock mehanics
#define TXD_PIN_ClockMeh (GPIO_NUM_34)//UART for rs485 connections to clock mehanics
#define RS485_PIN (GPIO_NUM_22) //Enable pin for rs485 connections to clock mehanics

#define CSSerial Serial1//rename UART for ClockStation
#define CMSerial Serial2//rename UART for ClockMehanics
#define SIMSerial Serial0
#define UARTtimeout 100 //time out of uart response in ms

static bool csUartTimeOut = false;
static bool cmUartTimeOut = false;


const char* host = "ClockStation";
const char* ssid = "wifi";
const char* Wpass = "F31wv32P";

#endif // __ESPCONFIG__
