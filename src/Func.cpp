#include "Func.h"


void serialFlush(HardwareSerial & port){
  while(port.available() > 0) {
    char t = port.read();
  }
}

std::string hexStr(unsigned char* data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss<<"0x" << std::setw(2) << std::setfill('0') << (int)data[i]<<" ";
    return ss.str();
}
std::string hexStr(const unsigned char* data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss<<"0x" << std::setw(2) << std::setfill('0') << (int)data[i]<<" ";
    return ss.str();
}


bool readUartUntil(HardwareSerial & port, unsigned char * answer, unsigned int TimeOut){
    bool firstTerminator = false;
    bool secondTrerminator = false;
    unsigned char answerLen = 0;
    
    answer[0]=0;
    int timeStart = millis();
    Serial.println( "func while began");
    while (TimeOut<millis() - timeStart)
    {
        if(port.available()>0){
            answerLen++;
            answer[0]= answerLen;           
            
            answer[answerLen]= port.read();
            Serial.println(answer[answerLen]);

            if(answer[answerLen]==0x10){
                firstTerminator = true;
            }
            else{
                firstTerminator = false;
            }
            if(firstTerminator and answer[answerLen]==0xfe){
                secondTrerminator = true;
            }
            if(secondTrerminator){
                // answer = (unsigned char *) realloc(answer, answerLen + 2);//// realloc cause core panic!!!!!!!!!!!!!!!!
                return false;
            }
        }
    }
    return true;
}


