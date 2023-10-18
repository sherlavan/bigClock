#include "Func.h"

std::string hexStr(unsigned char* data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss<<"0x" << std::setw(2) << std::setfill('0') << (int)data[i]<<" ";
    return ss.str();
}

static bool readUartUntil(HardwareSerial & port, unsigned char Terminator, unsigned char * answer, unsigned int TimeOut){

    unsigned char answerLen = 0;
    answer[0]=0;
    int timeStart = millis();

    while (TimeOut<millis() - timeStart)
    {
        if(port.available()>0){
            answerLen++;
            answer[answerLen]= port.read();

            if(answer[answerLen]==Terminator){
                answer[0]= answerLen;
                return false;
            }
        }
    }
    return true;
}