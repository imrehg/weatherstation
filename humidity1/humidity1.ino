static const int VCCPin = 8;
static const int analogPin = 0;
static const float Re = 50.3; // kOhm
//static const float Vref = 5.0 * 15 / (15 + 51);
static const float Vref = 5.0;
static const float Vm = 2.5;


static float a = 2.76366367;
static float b = -32.40924966;
static float c = 102.93566686;

int val = 0;
int count = 0;
unsigned long t;
boolean level = LOW;
unsigned long nowt;
static int microwait = 500;

void setup() {
  Serial.begin(19200);
  Serial.println(F("Humidity sensor"));
  pinMode(VCCPin, OUTPUT);
  t = micros();
}

void loop() {

  nowt = micros();
  if (nowt < t) { 
      t = 0;
  }
  if ((nowt - t) > microwait) {
     digitalWrite(VCCPin, level);
     if (level == HIGH) {
          if (count == 200) {
             Serial.println(GetHumidity(),3);
             count = 0;
          } else {
             count++; 
          }
     }
     level = !level;
     t = nowt;
  }  
}

float GetHumidity(void) {
  float hum;
  val = analogRead(analogPin);
  float volt = val * 5.0 / 1024;
  if (volt < Vm) {
    hum = 100.0;
  } else if (volt > Vref) {
    hum = 0.0;
  } else {
    float res = Re / ((Vref-Vm)/(volt-Vm) - 1);
    float reslog = log10(res);
    hum = a * reslog*reslog + b * reslog + c;
    if (hum > 100.0) {
       hum = 100.0;
    } else if (hum < 0.0) {
       hum = 0.0;
    }
  }
  return (hum);
}
