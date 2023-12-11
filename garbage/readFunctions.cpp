#include "readFunctions.h"



void readBaseParameters(serialPins UARTDev, Stream & OutputStream){
    bool csUartTimeOut;
    // prepeare command
    unsigned char *  command = buildCMD(ReadModelCMD, ParametrsCMD);

    uint8_t commandLen = command[0];
    uint8_t answerLen = command[commandLen-1];

    unsigned char * commandToSend = (unsigned char *) malloc(commandLen - 2);
    for (uint8_t i = 0; i<commandLen - 2; i++){
        commandToSend[i]=command[i+1];
    }
    //--prepeare
    Serial.println( "cmd to clock  from func");
    Serial.println(hexStr(commandToSend, commandLen - 2).c_str());

    //def answer and make 0000
    unsigned char * answer = (unsigned char *) malloc(255);
    for (uint8_t i = 0;i<answerLen;i++){
      answer[i]=0;
    }

    //start UART on needed pins
    Serial1.begin(UARTDev.baud, UARTDev.config, UARTDev.RXPIN, UARTDev.TXPIN);
    //clear buffer
    serialFlush(Serial1);
    csUartTimeOut = false;
    Serial1.write(commandToSend, commandLen - 2);
    unsigned long responseTime = millis();
    //release mem
    free(commandToSend);

    csUartTimeOut = readUartUntil(Serial1, 0xFE, answer);
    //release UART
    Serial1.end();
    Serial.println(hexStr(answer, answer[0]).c_str());
    //if timeout return info
    if (csUartTimeOut){ 
      StaticJsonDocument<30> answerJSON;
      answerJSON["UARTClockStationTimeOut"] = true;
      serializeJson(answerJSON, OutputStream);
      return;
    }

    if (answer[0]>5) {
      char chank[answer[0]-5];
      memcpy(chank, answer + 3, answer[0]-6);
      chank[answer[0]-6] = 0;//0 terminate
      StaticJsonDocument<300> answerJSON;
      answerJSON["ClockStationModel"] = chank;
      serializeJson(answerJSON, OutputStream);
      return;
    }

    
    StaticJsonDocument<40> answerJSON;
    answerJSON["UARTClockStation"] = "somthing went wrong!";
    serializeJson(answerJSON, OutputStream);
    return;


}