static const int VCCPin = 8;
static const int analogPin = 1;
static const int R = 15000;
static const float Vref = 5.0 * 15 / (15 + 51);

static float a = 2.76366367;
static float b = -32.40924966;
static float c = 102.93566686;

int val = 0;
int count = 0;
unsigned long t;
boolean level = LOW;
unsigned long nowt;

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
  if ((nowt - t) > 500) {
     digitalWrite(VCCPin, level);
     if (level == HIGH) {
          if (count == 200) {
             Serial.println(GetHumidity(),1);
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
  
  val = analogRead(analogPin);
  float volt = val * 5.0 / 1024;
  float res = log10(R / (Vref / volt - 1) / 1000);
  float hum = a * res*res + b * res + c;
  return (hum);
}
