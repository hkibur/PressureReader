#define LED 5
#define ADC A0

const int rollAvgBufLen = 5; //Length of rolling average buffer
int rollAvgBuf[rollAvgBufLen]; //Rolling average buffer

const int inRangeMin = 0; //Input range minimum
const int inRangeMax = 1024; //Input range maximum
const int outRangeMin = 0; //Output raange minimum
const int outRangeMax = 1024; //Output range maximum
float outMultiplier; //Multiplier for input value

int adcInput; //ADC read value, used in loop

void roll(int newVal){
  for(int i=0;i < rollAvgBufLen - 1;i++){
    rollAvgBuf[i] = rollAvgBuf[i + 1];
  }
  rollAvgBuf[rollAvgBufLen - 1] = newVal;
}

float compAvg(){
  float avg = 0.0;
  for(int i=0;i < rollAvgBufLen;i++){
    avg += rollAvgBuf[i];
  }
  return avg / (float)rollAvgBufLen; 
}

float getOutput(float val){
  if(val < inRangeMin){
    val = inRangeMin;
  } else if(val > inRangeMax){
    val = inRangeMax;
  }
  return (val - inRangeMin) * outMultiplier;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(ADC, INPUT);
  Serial.begin(9600);

  outMultiplier = (float)(outRangeMax - outRangeMin) / (float)(inRangeMax - inRangeMin);

  for(int i=0;i < rollAvgBufLen - 1;i++){
    rollAvgBuf[i] = analogRead(ADC);
  }
}

void loop() {
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  adcInput = analogRead(ADC);
  roll(adcInput);
  int rawOut = compAvg();
  Serial.println(getOutput(rawOut));
}
