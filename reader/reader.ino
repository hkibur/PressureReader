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

const int inRangeMin = 0; //Input range minimum
const int inRangeMax = 1024; //Input range maximum
const int outRangeMin = 0; //Output raange minimum
const int outRangeMax = 1024; //Output range maximum
float outMultiplier; //Multiplier for input value

const char* SSID = "abcdef";

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
	Serial.println("handled");
	server.send(200, "text/html", "<h1>You are connected</h1>");
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
}

void loop() {
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  adcInput = analogRead(ADC);
  avgBuf.shift(adcInput);
  Serial.println(getOutput(avgBuf.getAvg()));
}
