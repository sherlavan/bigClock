#define CONFIG_ETH_SPI_ETHERNET_W5500 1
#define ETH_TYPE        ETH_PHY_W5500
#define ETH_PHY_SPI_HOST    SPI2_HOST
#define ETH_ADDR         1
#define ETH_CS          5
#define ETH_IRQ          4
#define ETH_RST          22


// SPI pins
#define ETH_SPI_SCK     18
#define ETH_SPI_MISO    19
#define ETH_SPI_MOSI    23


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
#include <Arduino.h>

static bool eth_connected = false;



#define TCP_HOSTNAME           "192.168.88.24"
#define TCP_PORT               9999


// BluetoothSerial SBT;
WebServer server(80);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


uint32_t lastTcpPublishTime = 0;
uint8_t buffer[512];
serialPins clockStation;
serialPins clockMehanics;
serialPins sim800;

MobileInternet megafon;





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



void onEvent(arduino_event_id_t event, arduino_event_info_t info)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-eth0");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.printf("ETH Got IP: '%s'\n", esp_netif_get_desc(info.got_ip.esp_netif));
      
      eth_connected = true;
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}


static uint8_t sizeOfAnsver = 0;
void setup() {

WiFi.onEvent(onEvent);

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  ETH.begin(ETH_TYPE, ETH_ADDR, ETH_CS, ETH_IRQ, ETH_RST, SPI);
  // Ethernet.init(ETHERNET_CS_PIN);
  // Ethernet.begin(mac);
  megafon.APN = "internet";
  megafon.USR = "gdata";
  megafon.PAS = "gdata";

  clockStation.TXPIN = TXD_PIN_ClockStation;
  clockStation.RXPIN = RXD_PIN_ClockStation;
  clockStation.baud = 57600;
  clockStation.config = SERIAL_8N1;

  clockMehanics.TXPIN = TXD_PIN_ClockMeh;
  clockMehanics.RXPIN = RXD_PIN_ClockMeh;
  clockMehanics.baud = 9600;
  clockMehanics.config = SERIAL_8N1;

  sim800.TXPIN = TXD_PIN_Sim800;
  sim800.RXPIN = RXD_PIN_Sim800;
    

  pinMode(RS485_PIN_ClockMeh, OUTPUT);
  digitalWrite(RS485_PIN_ClockMeh, LOW);//set to recive 485 data
  Serial.begin(115200); // @todo reserved for sim900 module
  Serial1.setRxBufferSize(129);
  // Serial1.begin(57600, SERIAL_8N1, RXD_PIN_ClockStation, TXD_PIN_ClockStation); // uart1 with clock station

  // CSSerial.setRxBufferSize(129);// must be > 128 f.e. 129
  // CSSerial.begin(57600, SERIAL_8N1, RXD_PIN_ClockStation, TXD_PIN_ClockStation); // uart1 with clock station
  // CMSerial.setRxBufferSize(129);// must be > 128 f.e. 129
  // CMSerial.begin(9600, SERIAL_8N1);// uart2 with clock mehanics
  WiFi.onEvent(onEvent);

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  ETH.begin(ETH_TYPE, ETH_ADDR, ETH_CS, ETH_IRQ, ETH_RST, SPI);

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

if (eth_connected) {
    testClient("google.com", 80);
  }

  delay(5000);
}

