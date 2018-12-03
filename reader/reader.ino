#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LED 5
#define ADC A0

class RingBuffer {
protected:
    int _headPos;
    int _tailPos;
    int _bufLen;
    int* _buf;

    void _incPos(int &var);
public:
    RingBuffer(int len);
    void shift(int val);
    int getLen();
};

void RingBuffer::_incPos(int &var) {
    var += 1;
    if (var >= _bufLen) {
        var = 0;
    }
};

RingBuffer::RingBuffer(int len) {
    _buf = new int[len];
    _headPos = 0;
    _tailPos = len - 1;
    _bufLen = len;
};

void RingBuffer::shift(int val) {
  _incPos(_headPos);
  _incPos(_tailPos);
  _buf[_headPos] = val;
};

int RingBuffer::getLen() {
  return _bufLen;
};

class RAvgRingBuffer: public RingBuffer {
protected:
    float _curAvg;
public:
    RAvgRingBuffer(int len);
    float getAvg();
    void shift(int val);
};

RAvgRingBuffer::RAvgRingBuffer(int len) : RingBuffer(len){
  _curAvg = 0.0;
};

float RAvgRingBuffer::getAvg(){
  return _curAvg;
};

void RAvgRingBuffer::shift(int val) {
  _curAvg -= (float)_buf[_tailPos] / (float)_bufLen;
  _curAvg += (float)val / (float)_bufLen;
  _incPos(_headPos);
  _incPos(_tailPos);
  _buf[_headPos] = val;
};

RAvgRingBuffer avgBuf(10);
ESP8266WebServer server(80);
WiFiClient tcpSock;

const int inRangeMin = 0; //Input range minimum
const int inRangeMax = 1024; //Input range maximum
const int outRangeMin = 0; //Output raange minimum
const int outRangeMax = 1024; //Output range maximum
float outMultiplier; //Multiplier for input value

const char* SSID = "abcdef";
const char* HTML_RESP_FORM = "<!doctypehtml><form action=/ >SSID:<br><input name=ssid><br>Password:<br><input name=pass><br>Target:<br><input name=target><br><input type=submit value=Connect></form>";
const char* HTML_RESP_DONE = "<!doctypehtml><h1>All done</h1>";

const char* wifiSSID;
const char* wifiPass;
const char* wifiTarget;

int adcInput; //ADC read value, used in loop
bool serverSatisfied = false;

float getOutput(float val){
  if(val < inRangeMin){
    val = inRangeMin;
  } else if(val > inRangeMax){
    val = inRangeMax;
  }
  return (val - inRangeMin) * outMultiplier;
}

void serverHandleRoot() {
  if(server.hasArg("ssid") && server.hasArg("pass") && server.hasArg("target")){
    wifiSSID = server.arg("user").c_str();
    wifiPass = server.arg("pass").c_str();
    wifiTarget = server.arg("target").c_str();
    serverSatisfied = true;
    server.send(200, "text/html", HTML_RESP_DONE);
  } else {
    server.send(200, "text/html", HTML_RESP_FORM);
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(ADC, INPUT);
  Serial.begin(9600);

  outMultiplier = (float)(outRangeMax - outRangeMin) / (float)(inRangeMax - inRangeMin);

  for(int i=0; i < avgBuf.getLen(); i++){
    avgBuf.shift(analogRead(ADC));
  }

  WiFi.softAP(SSID);

  IPAddress ip = WiFi.softAPIP();
  Serial.println(ip);
  server.on("/", serverHandleRoot);
  server.begin();
  while (!serverSatisfied) {
	  server.handleClient();
  }
  server.close();
  WiFi.softAPdisconnect(true);
  WiFi.begin(wifiSSID, wifiPass);
  Serial.print("Connecting to wifi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
  tcpSock.connect("192.168.1.65", 111);
  Serial.println("connected to target");
}

void loop() {
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  adcInput = analogRead(ADC);
  avgBuf.shift(adcInput);
  tcpSock.println(getOutput(avgBuf.getAvg()));
  Serial.println(getOutput(avgBuf.getAvg()));
}
