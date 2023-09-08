#include "uartClockStationCommands.h"

uint8_t calculateLenOfCommand(const unsigned char * cmd, const uint8_t cmdLen){
    uint8_t result = cmdLen;

    for (uint8_t i = 0; i < cmdLen; i++){
        if(cmd[i]==0x10){
            result++;
        }
    }
    return result;
}


//Сборка полной команды для часовой станции, после отправки выполнить free(*fullCMD)
unsigned char * buildCMD(const unsigned char *cmd, uint8_t cmdLen, const unsigned char *startOfCommand, uint8_t sclen, const unsigned char *endC, uint8_t endCL){
    
    unsigned char * fullcmd = (unsigned char *) malloc(sclen + cmdLen + endCL + 1);//1 - Байт для контрольной суммы
    unsigned char controlSumm = 0x55;
    
    controlSumm^=startOfCommand[1];
    
    uint8_t i = 0;
    for (; i<sclen; i++){
        fullcmd[i] = startOfCommand[i];
    }

    uint8_t j = 0;
    for(; j<cmdLen; j++){
        fullcmd[i+j] = cmd[j];
        controlSumm^=cmd[j];
        if(cmd[j]==0x10){   //0x10 - spec, to send it, it must be duplicate
            i++;
            fullcmd[i+j] = 0x10;
        }
        
    }
    
    fullcmd[i+j]=controlSumm;
    uint8_t k = 0;
    for(; k < endCL;k++){
        fullcmd[i+j+1+k] = endC[k];
        
    }
    
    
    
 return fullcmd;   
}