#include <uartClockStationCommands.h>
#include <iostream>

using namespace std;

static const uint8_t StartCMD[] = {0x10, 0x01}; //start of packet
    static const uint8_t endOfCMD[] = {0x10, 0xFE}; //tail of packet for clock station
    static const uint8_t ReadParametrsCMD[] = {0x01};
    
    uint8_t*sc = StartCMD;
    uint8_t*tc = endOfCMD;
    uint8_t*rpc = ReadParametrsCMD;

int * buildCMD(int *startC = sc, int *cmd = rpc, int *endC = tc){
    for (int*elem=cmd;*elem<=cmd[1];elem++){
        cout<< *(elem)<<endl;
    }
    
}


int main()

{
    
    
    
    buildCMD();
    
    
    
    
    
    cout<<"Hello World";

    return 0;
}
