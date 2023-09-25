// #include <Arduino.h>
// #include "esp_system.h"
// #include "esp_log.h"
// #include "driver/uart.h"
// #include "string.h"
// #include "driver/gpio.h"
#include <inttypes.h>
#include <HardwareSerial.h>
#include "uartClockStationCommands.h"
#include "BluetoothSerial.h"

// static const int RX_BUF_SIZE = 1024;

//! For ESP32 c3 13 UART0 RXPIN = 20, TXPIN = 21
#define TXD_PIN_ClockStation (GPIO_NUM_22)//(GPIO_NUM_5) esp32c3
#define RXD_PIN_ClockStation (GPIO_NUM_23)//(GPIO_NUM_4) esp32c3

BluetoothSerial SBT;




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
//     uart_set_pin(UART, TXD_PIN_ClockStation, RXD_PIN_ClockStation, -1, -1);
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


static uint8_t sizeOfAnsver = 0;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); // 
  Serial1.setRxBufferSize(255);// must be > 128 f.e. 129
  Serial1.begin(57600, SERIAL_8N1, RXD_PIN_ClockStation, TXD_PIN_ClockStation); // uart1 with clock station
  SBT.begin("ClockStation");
  
}




void loop() {
  // unsigned char * answer = (unsigned char *) malloc(10);
  
  SBT.print("program start\r\n");

  unsigned char *  command = buildCMD(ReadSerialNCMD, ParametrsCMD);

  uint8_t commandLen = command[0];
  uint8_t answerLen = command[commandLen-1];


  SBT.println("commandLen");
  SBT.println(commandLen);

  SBT.println("answerLen");
  SBT.println(answerLen);

  SBT.println(" Data send ");
  // for(uint8_t i=0;i<commandLen;i++){
  //   SBT.print(command[i],HEX);
  //   SBT.print(" ");
  // }
  // SBT.println();

  unsigned char * commandToSend = (unsigned char *) malloc(commandLen - 2);
  for (uint8_t i = 0; i<commandLen - 2; i++){
    commandToSend[i]=command[i+1];
    SBT.print(commandToSend[i],HEX);
    SBT.print(" ");

  }
  SBT.println();


  

  
  // unsigned char *bytecmd;
  // bytecmd = 0;
  Serial1.write(commandToSend, commandLen - 2);
  unsigned long responseTime = millis();

  free(commandToSend);
  // delay(1000);
  
  unsigned char * answer = (unsigned char *) malloc(answerLen + 1);
  for (uint8_t i = 0;i<answerLen;i++){
    answer[i]=0;
  }
// @todo read string from bufer len = 255?
  while (Serial1.available()<answerLen)
  {
    delay(1);
  }
  
  if(Serial1.available()>=answerLen)  {
    // sizeOfAnsver = Serial1.available();
    // Serial.println(sizeOfAnsver);
    // free answer);
    SBT.print("Response time : ");
    Serial1.readBytesUntil(0xfe, answer, answerLen + 1);
    SBT.println(millis() - responseTime);

  }

  SBT.print("Recive response : ");
    for(uint8_t i = 0; i<answerLen; i++){
      SBT.print(answer[i], HEX);
      SBT.print(" ");
      }
    SBT.println("end Rx");

    free (answer);

      SBT.print(" End \r\n");
  delay(5000);

  // // Serial.println(*bytecmd);
  
  // // Serial1.readBytesUntil(0xFE answer,sizeof answer)-1);
      
  // if(sizeOfAnsver > 4){
  //   free(command);
    
  //   Serial.print("Recive response : ");
  //   for(u8_t i = 0; i<sizeOfAnsver; i++){
  //     Serial.print answer[i],HEX);
  //     Serial.print(" ");
  //     }
  //   Serial.println("end Rx");

  //   free answer);
  // }
  // byte command[] = {0x10, 0x01, 0x02, 0x56, 0x10, 0xFE};
  // byte command1[] = {0x10, 0x01, 0x09, 0x13, 0x50, 0x00, 0x06, 0x02, 0x09, 0x23,0x31,0x10,0xFE};
  // byte command2[] = {0x10, 0x01, 0x05, 0x51, 0x10, 0xFE};



  // Serial.print(" Data send ");
  // for (u8_t i = 0; i<sizeof(command1);i++){
  //   Serial.print(command1[i],HEX);
  //   Serial.print(",");
  // }
  // Serial.print("\r\n");



}

