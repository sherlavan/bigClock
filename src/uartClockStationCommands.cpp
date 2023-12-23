#include "uartClockStationCommands.h"

uint8_t calculateLenOfParams(const unsigned char * params, const uint8_t paramsLen){
    uint8_t result = 0;

    for (uint8_t i = 0; i < paramsLen; i++){
        if(params[i]==0x10){
            result++;
        }
    }
    return result;
}



unsigned char * buildCMD(const unsigned char *cmd, const unsigned char *startOfCommand, unsigned char *parametrs,  uint8_t sclen, const unsigned char *endC, uint8_t endCL){

    uint8_t cmdLen = cmd[0];
    uint8_t parametrsLenCorrection = calculateLenOfParams(parametrs,cmdLen - 1);


    
    unsigned char * fullcmd = (unsigned char *) malloc(sclen + cmdLen + parametrsLenCorrection + endCL + 1 + 1);//1 - Байт для контрольной суммы, 1 - если кс 0х10
    unsigned char controlSumm = 0x55;
    
    controlSumm^=startOfCommand[1];
    
    uint8_t i = 0;
    for (; i<sclen; i++){
        fullcmd[i+1] = startOfCommand[i];
    }

    fullcmd[i+1] = cmd[1];
    i++;
    controlSumm^=cmd[1];

    uint8_t j = 0;
    if(cmdLen>1){
        for(; j<cmdLen - 1; j++){
            fullcmd[i+j+1] = parametrs[j];
            controlSumm^=parametrs[j];
            if(parametrs[j]==0x10){   //0x10 - spec, to send it, it must be duplicate
                i++;
                fullcmd[i+j+1] = 0x10;
            }
            
        }
    }
    
    fullcmd[i+j+1] = controlSumm;
    j++;
    if(controlSumm == 0x10){
        fullcmd[i+j+1] = controlSumm;
        j++;
    }
    uint8_t k = 0;
    for(; k < endCL;k++){
        fullcmd[i+j+k+1] = endC[k];
        
    }
    fullcmd[i+j+k+1]=cmd[2];
    fullcmd[0] = i+j+1+k+1;

    
    
    
 return fullcmd;   
}