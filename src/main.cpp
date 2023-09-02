#include <Arduino.h>
// #include "esp_system.h"
// #include "esp_log.h"
// #include "driver/uart.h"
#include "string.h"
// #include "driver/gpio.h"
#include <inttypes.h>
#include <HardwareSerial.h>

// static const int RX_BUF_SIZE = 1024;

//! For ESP32 c3 13 UART0 RXPIN = 20, TXPIN = 21
#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_0)
// #define UART (UART_NUM_1)
// Led pins
#define WHITE_LED (GPIO_NUM_19)
#define ORANGE_LED (GPIO_NUM_18)
// 3 in one LEDS
#define RED_LED (GPIO_NUM_3)
#define GREEN_LED (GPIO_NUM_4)
#define BLUE_LED (GPIO_NUM_5)



// void init(void) {
//     const uart_config_t uart_config = {
//         .baud_rate = 57600,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
//     };
//     // We won't use a buffer for sending data.
//     uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
//     uart_param_config(UART, &uart_config);
//     uart_set_pin(UART, TXD_PIN, RXD_PIN, -1, -1);
// }

// int sendData( const char* data)
// {
//     const int len = strlen(data);
//     const int txBytes = uart_write_bytes(UART, data, len);
//     // ESP_LOGI("Wrote %d bytes", txBytes);
//     return txBytes;
// }


// char* reciveData(void){
// char* data = (char*) malloc(RX_BUF_SIZE+1);
// const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, 50 / portTICK_PERIOD_MS);
// if (rxBytes > 0) {
//     data[rxBytes] = 0;
//     ESP_LOGI( "Read %d bytes: '%s'", rxBytes, data);
// }

// return data;
// }



void setup() {
  // put your setup code here, to run once:
  pinMode(WHITE_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  Serial.begin(115200); // 
  Serial1.setRxBufferSize(130);// must be > 128 f.e. 129
  Serial1.begin(57600, SERIAL_8N1, 0, 1); // uart1 with clock station
  static byte commandCS[20]; // store computed command with check summ and end of packet
  
}

byte countCheckSumm(const byte *cmd[]){
  byte cs = 0x55;
  if (sizeof(cmd)>1){
    // commandCS[0] = 1; todo
    for(u8_t i = 1; i < sizeof(cmd); i++){
      cs^=*cmd[i];
    }
  }
  return cs;
}




void loop() {
  
  Serial.print("program start\r\n");
  // init();

  // byte command[] = {0x10, 0x01, 0x02};

  
  byte command[] = {0x10, 0x01, 0x02, 0x56, 0x10, 0xFE};
  byte command1[] = {0x10, 0x01, 0x09, 0x13, 0x50, 0x00, 0x06, 0x02, 0x09, 0x23,0x31,0x10,0xFE};
  byte command2[] = {0x10, 0x01, 0x05, 0x51, 0x10, 0xFE};
//ЧТЕНИЯ ДЛИТЕЛЬНОСТИ ИМПУЛЬСА В КАНАЛАХ
//   Запрос: 0x10 ADR CMD CS 0x10 0xFE (10 01 02 56 10 FE)
// ADR – адрес ЧС; CMD – 0x02
// CS = 0x55^ADR^CMD
// Ответ: 0x10 ADR
// 1. Один байт длительности импульса. Число в десятых долях секунды. Предельные
// значения от 2 до 240, что соответствует длительности импульса от 0,2 до 24,0 секунды.
// CS 0x10 0xFE.
//
  char name_arr[21];

  Serial1.write(command1, sizeof(command1));
  if(Serial1.available()){
    Serial1.readBytesUntil(0xFE,name_arr,sizeof(name_arr)-1);
  }

  Serial.print(" Data send ");
  for (u8_t i = 0; i<sizeof(command1);i++){
    Serial.print(command1[i],HEX);
    Serial.print(",");
  }
  Serial.print("\r\n");

  Serial.print("Recive response : ");
  for(u8_t i = 0; i<sizeof(name_arr); i++){
    Serial.print(name_arr[i],HEX);
    Serial.print(",");
    }

  Serial.print(" End \r\n");

 
  digitalWrite(BLUE_LED, 1);
  digitalWrite(GREEN_LED, 1);
  digitalWrite(RED_LED, 1);
  delay(500);
  digitalWrite(BLUE_LED, 0);
  digitalWrite(GREEN_LED, 0);
  digitalWrite(RED_LED, 0);
  delay(1000);


}
