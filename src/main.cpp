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
#include "config.h"


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

  pinMode(RS485_PIN,OUTPUT);
  digitalWrite(RS485_PIN,LOW);
  Serial.begin(115200); // @todo reserved for sim900 module
  CSSerial.setRxBufferSize(255);// must be > 128 f.e. 129
  CSSerial.begin(57600, SERIAL_8N1, RXD_PIN_ClockStation, TXD_PIN_ClockStation); // uart1 with clock station
  CMSerial.setRxBufferSize(255);// must be > 128 f.e. 129
  CMSerial.begin(9600, SERIAL_8N1);// uart2 with clock mehanics
  SBT.begin("ClockStation"); //BT Name
  
}

///repo test


void loop() {

  
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
  CSSerial.write(commandToSend, commandLen - 2);
  unsigned long responseTime = millis();

  free(commandToSend);
  // delay(1000);
  
  unsigned char * answer = (unsigned char *) malloc(answerLen + 1);
  for (uint8_t i = 0;i<answerLen;i++){
    answer[i]=0;
  }
// @todo read string from bufer len = 255?
  while (CSSerial.available()<answerLen)
  {
    delay(1);
  }
  
  if(CSSerial.available()>=answerLen)  {
    // sizeOfAnsver = Serial1.available();
    // Serial.println(sizeOfAnsver);
    // free answer);
    SBT.print("Response time : ");
    CSSerial.readBytesUntil(0xfe, answer, answerLen + 1);
    SBT.println(millis() - responseTime);

  }

  SBT.print("Recive response : ");
    for(uint8_t i = 0; i<answerLen; i++){
      SBT.print(answer[i], HEX);
      SBT.print(" ");
      }
  SBT.println("end Rx");

  free (answer);

  SBT.println(" vvv CM vvv");

  static const unsigned char mehParams [] = {0x10, 0x02, 0x50, 0x07, 0x10, 0xFE};

  digitalWrite(RS485_PIN,HIGH);//enable 485 transmition
  CMSerial.write(mehParams,6);
  digitalWrite(RS485_PIN,LOW);//enable 485 recive
  delay(5);
  unsigned char * cmanswer = (unsigned char *) malloc(11);
  for (uint8_t i = 0;i<11;i++){
    cmanswer[i]=0;
  }
  while (CMSerial.available()<10)
  {
    delay(1);
  }
  if(CMSerial.available()>0){
    CMSerial.readBytes(cmanswer,10);
  }
  for(uint8_t i = 0; i<11; i++){
    SBT.print(cmanswer[i],HEX);

  }
  SBT.println("");
  SBT.println(" ^^^ CM ^^^");
  free(cmanswer);
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

