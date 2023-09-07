#include <iostream>

using namespace std;

static const unsigned char StartCMD[] = {0x10, 0x01}; //start of packet
static const unsigned char endOfCMD[] = {0x10, 0xFE, 0x00}; //tail of packet for clock station
static const unsigned char ReadParametrsCMD[] = {0x01};//could be 
static const uint8_t StartCMDLen = sizeof(StartCMD)/sizeof(StartCMD[0]);
static const uint8_t endOfCMDLen = sizeof(endOfCMD)/sizeof(endOfCMD[0]);
static const uint8_t ReadParametrsCMDLen = sizeof(ReadParametrsCMD)/sizeof(ReadParametrsCMD[0]);

uint8_t calculateLenOfCommand(const unsigned char * cmd){
    uint8_t len, result = sizeof(cmd)/sizeof(cmd[0]);
    // unsigned char result = len;
    for (uint8_t i = 0; i < len; i++){
        if(cmd[i]==0x10){
            result++;
        }
    }
    return result;
}


unsigned char * buildCMD(const unsigned char *startC = StartCMD, uint8_t sclen = StartCMDLen, const unsigned char *cmd = ReadParametrsCMD, uint8_t cmdLen = ReadParametrsCMDLen, const unsigned char *endC = endOfCMD, uint8_t endCL = endOfCMDLen){
    unsigned char * fullcmd = (unsigned char *) malloc(sclen + cmdLen + endCL + 1);//1 - controlSumm
    unsigned char controlSumm = 0x55;
    controlSumm^=startC[1];
    uint8_t i = 0;
    for (; i<sclen; i++){
        cout<<hex<<fullcmd[i];
        cout<<hex<<startC[i];
        fullcmd[i] = startC[i];

    }

    uint8_t j = 0;
    for(; j<cmdLen; j++){
        fullcmd[i+j] = cmd[j];
        controlSumm^=cmd[j];
        
    }
    
    fullcmd[i+j]=controlSumm;
    uint8_t k = 0;
    for(; k < endCL;k++){
        fullcmd[i+j+1+k] = endC[k];
        
    }
    
    fullcmd[i+j+2+k] = 0x00;
    
 return fullcmd;   
}


int main()

{
    static const unsigned char WriteDateTimeCMD[] = {0x09,0x12,0x00,0x00,0x07,0x01,0x10,0x23};
    // cout<<std::hex<<0x10<<endl;
    unsigned char* rrrr = buildCMD();
    
    for(uint8_t i=0;i<5;i++){
        cout<<hex<<(int)rrrr[i];
        cout<<";";
    }
    cout<<endl;
    
    for(uint8_t i=0;i<sizeof(WriteDateTimeCMD)/sizeof(WriteDateTimeCMD[0]);i++){
        cout<<hex<<(int)WriteDateTimeCMD[i];
        cout<<";";
    }
    cout<<endl;
    uint8_t l = calculateLenOfCommand(WriteDateTimeCMD);
    
    cout<<(int)l<<endl;
    

    return 0;
}