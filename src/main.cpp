#define ARDUINOJSON_DECODE_UNICODE 0
#include "config.h"
#include "uartClockStationCommands.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <Update.h>
#include <WebServer.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include "Func.h"
#include <string>
#include <map>
#include <EthernetClient.h>

//#########################################   Constant block  ###############################

const char version[6+1] =
{
   // YY year
   __DATE__[9], __DATE__[10],

   // First month letter, Oct Nov Dec = '1' otherwise '0'
   (__DATE__[0] == 'O' || __DATE__[0] == 'N' || __DATE__[0] == 'D') ? '1' : '0',
   
   // Second month letter
   (__DATE__[0] == 'J') ? ( (__DATE__[1] == 'a') ? '1' :       // Jan, Jun or Jul
                            ((__DATE__[2] == 'n') ? '6' : '7') ) :
   (__DATE__[0] == 'F') ? '2' :                                // Feb 
   (__DATE__[0] == 'M') ? (__DATE__[2] == 'r') ? '3' : '5' :   // Mar or May
   (__DATE__[0] == 'A') ? (__DATE__[1] == 'p') ? '4' : '8' :   // Apr or Aug
   (__DATE__[0] == 'S') ? '9' :                                // Sep
   (__DATE__[0] == 'O') ? '0' :                                // Oct
   (__DATE__[0] == 'N') ? '1' :                                // Nov
   (__DATE__[0] == 'D') ? '2' :                                // Dec
   0,

   // First day letter, replace space with digit
   __DATE__[4]==' ' ? '0' : __DATE__[4],

   // Second day letter
   __DATE__[5],

  '\0'
};

const int sw_version = atoi(version);

const serialPins clockStation = {.TXPIN = TXD_PIN_ClockStation,
                                  .RXPIN = RXD_PIN_ClockStation,
                                  .baud = 57600,
                                  .config = SERIAL_8N1};

const serialPins clockMehanics = {.TXPIN = TXD_PIN_ClockMeh,
                                  .RXPIN = RXD_PIN_ClockMeh,
                                  .baud = 9600,
                                  .config = SERIAL_8N1};
                                  
const serialPins sim800 = {.TXPIN = TXD_PIN_Sim800,
                      .RXPIN = RXD_PIN_Sim800};

MobileInternet megafon = {.APN = "internet",
                          .USR = "gdata",
                          .PAS = "gdata",};


//dictionary  of commands and devices
std::map<std::string, const unsigned char* > commandsToClockStationArray = {
    {"MelodyTest", MelodyTestCMD},
    {"ChimesTest", ChimesTestCMD},
    {"ReadParametrs", ReadParametrsCMD},
    {"ReadImpulse", ReadImpulseCMD},
    {"ReadGlonas", ReadGlonasCMD},
    {"ReadDayCorrection", ReadDayCorrectionCMD},
    {"WriteTimeInChannel", WriteTimeInChannelCMD},
    {"WriteImpulse",WriteImpulseCMD},
    {"WriteImpulseDurationInChannel", WriteImpulseDurationInChannelCMD},
    {"WriteDateTime", WriteDateTimeCMD},
    {"WriteDayCorrection", WriteDayCorrectionCMD},
    {"WriteTimeZone", WriteTimeZoneCMD},
    {"ReadReleyState", ReadReleyStateCMD},
    {"ReadAstronomicReley", ReadAstronomicReleyCMD},
    {"WriteReley", WriteReleyCMD},
    {"WriteAstronomicReley", WriteAstronomicReleyCMD},
    {"WriteCoordinates", WriteCoordinatesCMD},
    {"ReadModel", ReadModelCMD},
    {"ReadVersion", ReadVersionCMD},
    {"ReadSerialN", ReadSerialNCMD},
    {"ReadCSN", ReadCSNCMD},
    {"ReadWarranty", ReadWarrantyCMD},
    {"WriteSerialN", WriteSerialNCMD},
    {"WriteCSN", WriteCSNCMD},
    {"WriteWarranty", WriteWarrantyCMD},
    {"ReadDemo", ReadDemoCMD},
    {"WriteDemo", WriteDemoCMD},
    {"ReadChimes", ReadChimesCMD},
    {"WriteChimesTime", WriteChimesTimeCMD},
    {"WriteChimes", WriteChimesCMD},
    {"WriteChimesVolume", WriteChimesVolumeCMD}

};

std::map<std::string, const serialPins& > deviceArray = {
  {"ClockStation", clockStation},
  {"ClockMehanics", clockMehanics},
  {"Sim", sim800} //check if needed
};


//#################################3 varables   ################################
std::string TestData = "";

bool eth_connected = false;
bool BTClientConnected = false;
char serverURL[] = "arduino.tips";
char BTsendBufer[1024];
String BTreciveBufer;


BluetoothSerial BTSerial;
EthernetClient EClient;
WebServer server(80);


//############################## html block  ####################################
const char* UpdatePage = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
  "$('form').submit(function(e){e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  "$.ajax({url: '/update', type: 'POST', data: data, contentType: false, processData:false, xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
"</script>";


//#####################################  Function block   ################################################### 
void queryModule(serialPins UARTDev,const unsigned char* command, char * OutputStream);

void printoutData()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (EClient.available())
  {
    char c = EClient.read();
    Serial.write(c);
    Serial.flush();
  }
}

void BTCheckClientConnection(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    BTClientConnected = true;
    Serial.println("Client Connected to Bluetooth");
  }
  if(event == ESP_SPP_SRV_STOP_EVT){
    BTClientConnected = false;
    Serial.println("Client Disconnected from Bluetooth");
  }
}

void handleBTRequest();


//################################ setup block   #####################################################
void setup() {
  BTSerial.begin("CS-BK - xxxxxxTest");
  BTSerial.register_callback(BTCheckClientConnection);
  Serial.begin(115200); 
  Serial.println("Starting Setup stage");

  Ethernet.init (ETH_CS);
  EthernetLinkStatus link = Ethernet.linkStatus();
  if(link == Unknown){
    Serial.println("Problem with eathernet module..., not initialized");
  }

  if(link == LinkOFF){
    Serial.println("Cabel unpluged");
  }

    // start the ethernet connection and the server:
    // Use DHCP dynamic IP and random mac
    uint16_t index = millis() % NUMBER_OF_MAC;
    // Use Static IP
    //Ethernet.begin(mac[index], ip);
    Ethernet.begin(mac[index]);
    Serial.print("Using mac: ");
    for(int i =0;i<6;i++){
      Serial.print(mac[index][i]);
      Serial.print(":");
    }
    Serial.println();

    Serial.print("Got IP address: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Connection speed: ");
    Serial.println(Ethernet.speedReport());
    Serial.print(F(", Duplex: "));
    Serial.println(Ethernet.duplexReport());
    Serial.print(F(", Link status: "));
    Serial.println(Ethernet.linkReport());

    if (EClient.connect(serverURL, 80))
    {
      Serial.println(F("Connected to server"));
      // Make a HTTP request
      EClient.println(F("GET /asciilogo.txt HTTP/1.1"));
      EClient.println(F("Host: arduino.tips"));
      EClient.println(F("Connection: close"));
      EClient.println();
    }
    else
    {
      Serial.println("Could not connect to given URL:");
    }


    pinMode(RS485_PIN_ClockMeh, OUTPUT);
    digitalWrite(RS485_PIN_ClockMeh, LOW);//set to recive 485 data
    
    Serial1.setRxBufferSize(129);


    WiFi.begin(ssid, Wpass);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(WiFi.status());
      delay(100);
    }
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.RSSI());
    Serial.println(WiFi.macAddress());


    server.on("/serverIndex", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", UpdatePage);
    });

  

    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        Serial.println("Update FW is in progress...");
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.println("update finish, restarting..");
        } else {
          Update.printError(Serial);
        }
      }
    });
    Serial.println("webserver settings done..");
    server.begin();
    Serial.println("webserver start..");

}




void loop() {


  // if the server's disconnected, stop the client
  if (!EClient.connected())
  {
    printoutData();

    Serial.println();
    Serial.println(F("Disconnecting from server..."));
    EClient.stop();
  }
  TestData = "";
  server.handleClient();
  Serial.println("server Ready");

  unsigned char *  command = buildCMD(ReadParametrsCMD, StartCMD, ParametrsCMD);

  uint8_t commandLen = command[0];
  uint8_t answerLen = command[commandLen-1];


  unsigned char * commandToSend = (unsigned char *) malloc(commandLen - 2);
  for (uint8_t i = 0; i<commandLen - 2; i++){
    commandToSend[i]=command[i+1];
  
  }

  TestData += "cmd to clock \n";
  TestData += hexStr(commandToSend, commandLen - 2);
  TestData += "\n";


  csUartTimeOut = false;
  
  Serial1.begin(clockStation.baud, clockStation.config, clockStation.RXPIN, clockStation.TXPIN);
  Serial1.write(commandToSend, commandLen - 2);
  unsigned long responseTime = millis();

  free(commandToSend);

  
  unsigned char * answer = (unsigned char *) malloc(answerLen + 1);
  for (uint8_t i = 0;i<answerLen;i++){
    answer[i]=0;
  }
// @todo read string from bufer len = 255?

  while (Serial1.available()<answerLen)
  {
    delay(1);
    if ((millis() - responseTime)> UARTtimeout){
      csUartTimeOut = true;
      break;
    }
  }
  
  if(Serial1.available()>=answerLen and !csUartTimeOut)  {
   
    Serial1.readBytesUntil(0xfe, answer, answerLen + 1);

  }

  Serial1.end();
  TestData += "cs uart time out: ";
  TestData += (csUartTimeOut ? "Yes\n" : "No\n");
  TestData += "Answer from clock\n";
  TestData += hexStr(answer, answerLen + 1);
  TestData += "\n";
  TestData += "Response Time: ";
  TestData += millis() - responseTime;
  TestData += "\n";


  free (answer);

  static unsigned char mehParams [] = {0x10, 0x02, 0x50, 0x07, 0x10, 0xFE};
  TestData += "\ncmd to meh: \n";
  TestData += hexStr(mehParams, 6);

  digitalWrite(RS485_PIN_ClockMeh, HIGH);//enable 485 transmition
  serialFlush(Serial1);
  Serial1.begin(clockMehanics.baud, clockMehanics.config, clockMehanics.RXPIN, clockMehanics.TXPIN);
  Serial1.write(mehParams,6);
  digitalWrite(RS485_PIN_ClockMeh, LOW);//enable 485 recive
  cmUartTimeOut = false;
  unsigned char * cmanswer = (unsigned char *) malloc(16);
  for (uint8_t i = 0;i<15;i++){
    cmanswer[i]=0;
  }
  responseTime = millis();
  while (Serial1.available()<15)//answerlen
  {
    delay(1);
    if ((millis() - responseTime)> UARTtimeout){
      cmUartTimeOut = true;
      serialFlush(Serial1);
      break;
    }
  }

  if(Serial1.available()>0 and !cmUartTimeOut){
    Serial1.readBytes(cmanswer,15);
  }
  Serial1.end();

  TestData += "Meh time out: ";
  TestData += (cmUartTimeOut ? "Yes\n" : "NO\n");
  TestData += "Answer from meh\n";
  TestData += hexStr(cmanswer, 15);
  TestData += "\n";
  

  free(cmanswer);
  delay(1000);
  Serial.println(TestData.c_str());
  server.on("/testdata", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", TestData.c_str());
  });

  queryModule(clockStation,ReadModelCMD, BTsendBufer);
  
  BTSerial.println(BTsendBufer);
  Serial.print("Uptime: ");
  Serial.println(millis());

  delay(5000);
}
// Functions block   vvvvvvvvvvvvvvvvvvvvvvvvvvvv

void queryModule(serialPins UARTDev,const unsigned char* sendCommand, char * OutputStream){
    bool csUartTimeOut;
    // prepeare command
    unsigned char *  command = buildCMD(sendCommand, StartCMD, ParametrsCMD);

    uint8_t commandLen = command[0];
    uint8_t answerLen;
    if(command[commandLen-1] == 0){
      answerLen = 100;
    }
    else{
      answerLen = command[commandLen-1];
    }
        

    unsigned char * commandToSend = (unsigned char *) malloc(commandLen - 2);
    for (uint8_t i = 0; i<commandLen - 2; i++){
        commandToSend[i]=command[i+1];
    }
    //--prepeare
    Serial.println( "cmd to clock  from func");
    Serial.println(hexStr(commandToSend, commandLen - 2).c_str());

    //def answer and make 0000
    unsigned char * answerUART = (unsigned char *) malloc(answerLen + 1);
    for (uint8_t i = 0;i<answerLen;i++){
      answerUART[i]=0;
    }

    //start UART on needed pins
    Serial1.begin(UARTDev.baud, UARTDev.config, UARTDev.RXPIN, UARTDev.TXPIN);
    //clear buffer
    serialFlush(Serial1);
    csUartTimeOut = false;
    Serial1.write(commandToSend, commandLen - 2);
    //release mem
    free(commandToSend);
    //reset response timer
    unsigned long responseTime = millis();

    while (Serial1.available()<6)
    {
      delay(1);
      if ((millis() - responseTime)> UARTtimeout){
        csUartTimeOut = true;
        break;
      }
    }
    
    if(Serial1.available()>=6 and !csUartTimeOut)  {
   
      Serial1.readBytesUntil(0xfe, answerUART, answerLen);

    }
    responseTime = millis() - responseTime;

    // csUartTimeOut = readUartUntil(Serial1, 0xFE, answerUART);
    //release UART
    Serial1.end();
    Serial.println(hexStr(answerUART, answerLen).c_str());
    //if timeout return info

    if (answerUART[0]>5) {
      char chank[answerLen-5];
      memcpy(chank, answerUART + 3, answerLen-5);
      chank[answerLen-6] = 0;//0 terminate
      Serial.println(chank);
      StaticJsonDocument<300> answerJSON;
      answerJSON["CSModel"] = chank;
      answerJSON["Response"] = responseTime;
      answerJSON["UARTCSTimeOut"] = (csUartTimeOut)?1:0;
      serializeJson(answerJSON, OutputStream, 300);
      Serial.println("test send done?");
      return;
    }

    
    StaticJsonDocument<200> answerJSON;
    answerJSON["Error"] = "UART answer is less then 6 byte!";
    answerJSON["Device"] = "DeviceVar here";
    answerJSON["Command"] = "CommandVar here";
    answerJSON.createNestedArray("Params");
    for(int i=0;i<ParametrsCMD[0];i++){
      answerJSON["Params"].add(ParametrsCMD[i+1]);
    }
    answerJSON["Response"] = hexStr(answerUART, answerLen).c_str();
    answerJSON["UARTCSTimeOut"] = (csUartTimeOut)?1:0;

    serializeJson(answerJSON, OutputStream, 200);
    return;


}

void handleBTRequest(){
    if(!BTClientConnected){
      return;
    }
    if(BTSerial.available()>20){//wait JSON string
      BTreciveBufer = BTSerial.readString();
    }
  return;
}