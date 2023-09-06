#include <iostream>

using namespace std;

static const unsigned char StartCMD[] = {0x10, 0x01}; //start of packet
static const unsigned char endOfCMD[] = {0x10, 0xFE}; //tail of packet for clock station
static const unsigned char ReadParametrsCMD[] = {0x01};//could be 
static const unsigned char StartCMDLen = sizeof(StartCMD)/sizeof(StartCMD[0]);
static const unsigned char endOfCMDLen = sizeof(endOfCMD)/sizeof(endOfCMD[0]);
static const unsigned char ReadParametrsCMDLen = sizeof(ReadParametrsCMD)/sizeof(ReadParametrsCMD);


unsigned char * buildCMD(const unsigned char *startC = StartCMD, unsigned char sclen = StartCMDLen, const unsigned char *cmd = ReadParametrsCMD, unsigned char cmdLen = ReadParametrsCMDLen, const unsigned char *endC = endOfCMD, unsigned char endCL = endOfCMDLen){
    unsigned char * fullcmd[sclen + cmdLen + endCL + 2];//2 - controlSumm + stopBit 0x00
    unsigned char controlSumm = 0x55;
    controlSumm^=startC[1];
    uint8_t i = 0;
    for (; i<sclen; i++){
        cout<<hex<<fullcmd[i];
        cout<<hex<<startC[i];
        *fullcmd[i] = startC[i];

    }

    uint8_t j = 0;
    for(; j<cmdLen; j++){
        *fullcmd[i+j] = cmd[j];
        controlSumm^=cmd[j];
        
    }
    
    *fullcmd[i+j]=controlSumm;
    uint8_t k = 0;
    for(; k < endCL;k++){
        *fullcmd[i+j+1+k] = endC[k];
        
    }
    
    *fullcmd[i+j+2+k] = 0x00;
    
 return *fullcmd;   
}


int main()

{
    static const unsigned char WriteDateTimeCMD[] = {0x09,0x12,0x00,0x00,0x07,0x01,0x09,0x22};
    // cout<<std::hex<<0x10<<endl;
    unsigned char* rrrr = buildCMD();
    
    for(uint8_t i=0;i<5;i++){
        cout<<hex<<rrrr[i];
        cout<<";";
    }
    cout<<endl;
    
    for(uint8_t i=0;i<sizeof(WriteDateTimeCMD)/sizeof(WriteDateTimeCMD[0]);i++){
        cout<<hex<<WriteDateTimeCMD[i];
        cout<<";";
    }
    

    return 0;
}