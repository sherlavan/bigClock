// #include <Arduino.h>
// #include "esp_system.h"
// #include "esp_log.h"
// #include "driver/uart.h"
// #include "string.h"
// #include "driver/gpio.h"

#include <inttypes.h>
#include "uartClockStationCommands.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include <ETH.h>
#include "Func.h"
#include <string>
#include <SPI.h>
#include <Ethernet_Generic.h>

#define MAX_SOCK_NUM 8

#define USE_TWO_ETH_PORTS 0
#define ETHERNET_RESET_PIN      22      // ESP32 pin where reset pin from W5500 is connected
#define ETHERNET_CS_PIN         5       // ESP32 pin where CS pin from W5500 is connected


#define TCP_HOSTNAME           "192.168.88.24"
#define TCP_PORT               9999


// BluetoothSerial SBT;
WebServer server(80);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

uint32_t lastTcpPublishTime = 0;
uint8_t buffer[512];


std::string TestData = "";

const char* UpdatePage = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
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


/*
 * Wiz W5500 reset function
 */
void ethernetWizReset(const uint8_t resetPin) {
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(250);
    digitalWrite(resetPin, LOW);
    delay(50);
    digitalWrite(resetPin, HIGH);
    delay(350);
}

void connectEthernet() {
    delay(500);


    Ethernet.init(ETHERNET_CS_PIN);
    ethernetWizReset(ETHERNET_RESET_PIN);

    Ethernet.begin(mac,10000,1000);
    delay(200);

}

void connectToServer() {
    Serial.println("Connecting to TCP server...");
    while (!ethClient.connect(TCP_HOSTNAME, TCP_PORT))  {
      Serial.println("Connection failed. Reconnecting...");
      delay(1000);
    }
    Serial.println("Connected to TCP server");
}

static uint8_t sizeOfAnsver = 0;
void setup() {

  // Ethernet.init(ETHERNET_CS_PIN);
  // Ethernet.begin(mac);
  pinMode(RS485_PIN,OUTPUT);
  digitalWrite(RS485_PIN,LOW);
  Serial.begin(115200); // @todo reserved for sim900 module
  CSSerial.setRxBufferSize(129);// must be > 128 f.e. 129
  CSSerial.begin(57600, SERIAL_8N1, RXD_PIN_ClockStation, TXD_PIN_ClockStation); // uart1 with clock station
  CMSerial.setRxBufferSize(129);// must be > 128 f.e. 129
  CMSerial.begin(9600, SERIAL_8N1);// uart2 with clock mehanics


  WiFi.begin(ssid, Wpass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(WiFi.status());
    delay(100);
  }
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI());


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

  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);
  Serial.println(Ethernet.getChip());
  W5100Class w55;
  Serial.println(w55.getChip());

  connectEthernet();
  Serial.println("Eth init pass?");
  Serial.println(Ethernet.linkReport());
}

///repo test


void loop() {
  TestData = "";
  server.handleClient();
  Serial.println("server Ready");

  unsigned char *  command = buildCMD(ReadSerialNCMD, ParametrsCMD);

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
  CSSerial.write(commandToSend, commandLen - 2);
  unsigned long responseTime = millis();

  free(commandToSend);

  
  unsigned char * answer = (unsigned char *) malloc(answerLen + 1);
  for (uint8_t i = 0;i<answerLen;i++){
    answer[i]=0;
  }
// @todo read string from bufer len = 255?

  while (CSSerial.available()<answerLen)
  {
    delay(1);
    if ((millis() - responseTime)> UARTtimeout){
      csUartTimeOut = true;
      break;
    }
  }
  
  if(CSSerial.available()>=answerLen and !csUartTimeOut)  {
   
    CSSerial.readBytesUntil(0xfe, answer, answerLen + 1);

  }
  TestData += "cs uart time out: ";
  TestData += (csUartTimeOut ? "Yes\n" : "No\n");
  TestData += "Answer from clock\n";
  TestData += hexStr(answer, answerLen + 1);
  TestData += "\n";
  TestData += "Response Time: ";
  TestData += millis() - responseTime;
  TestData += "\n";


  // SBT.print("Recive response : ");
    for(uint8_t i = 0; i<answerLen; i++){
    
      // SBT.print(answer[i], HEX);
      // SBT.print(" ");
      }
  // SBT.println("end Rx");

  free (answer);

  static unsigned char mehParams [] = {0x10, 0x02, 0x50, 0x07, 0x10, 0xFE};
  TestData += "\ncmd to meh: \n";
  TestData += hexStr(mehParams, 6);

  digitalWrite(RS485_PIN,HIGH);//enable 485 transmition
  serialFlush(CMSerial);
  CMSerial.write(mehParams,6);
  digitalWrite(RS485_PIN,LOW);//enable 485 recive
  cmUartTimeOut = false;
  unsigned char * cmanswer = (unsigned char *) malloc(16);
  for (uint8_t i = 0;i<15;i++){
    cmanswer[i]=0;
  }
  responseTime = millis();
  while (CMSerial.available()<15)
  {
    delay(1);
    if ((millis() - responseTime)> UARTtimeout){
      cmUartTimeOut = true;
      serialFlush(CMSerial);
      break;
    }
  }

  if(CMSerial.available()>0 and !cmUartTimeOut){
    CMSerial.readBytes(cmanswer,15);
  }
  serialFlush(CMSerial);

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

ethClient.write(TestData.c_str());

CMSerial.flush();
CSSerial.flush();
if (ethClient.available()) {
    char c = ethClient.read();
    Serial.print(c);
  }


}

