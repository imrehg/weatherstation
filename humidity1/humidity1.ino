#include <limits.h>  // For ULONG_MAX

static const int VCCPin = 9;
static const int analogPin = 0;
static const int tempPin = 4;
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
static unsigned long microwait = 500;
unsigned long deltat;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Humidity sensor"));
  pinMode(VCCPin, OUTPUT);
  t = micros();
}

void loop() {
  float temp, hum, res;
  
  nowt = micros();
  if (nowt >= t) {
    deltat = nowt - t;
  } 
  else {
    // Taking care of micros overflow that should happen every 70 minutes or so
    deltat = ULONG_MAX - t + nowt;
  }
  if (deltat > microwait) {
    digitalWrite(VCCPin, level);
    if (level == HIGH) {
      if (count == 400) {
        res = GetResistance();

        digitalWrite(VCCPin, LOW);
        delay(50);
        temp = GetTemperature();
        digitalWrite(VCCPin, HIGH);
        
        hum = GetHumidity(res, temp);
        Serial.print(hum);
        Serial.print(",");
        Serial.print(res);
        Serial.print(",");
        Serial.print(temp);
        Serial.println();
        count = 0;
      } else {
        count++; 
      }
    }
    level = !level;
    t = nowt;
  }  
}

// Output resistance in kOhm
float GetResistance(void) {
  float resistance, volt;
  int val;
  
  val = analogRead(analogPin);
  volt = val * 5.0 / 1024;

  if (volt < Vm) {
    resistance = -1;
  } 
  else if (volt > Vref) {
    resistance = -2;
  } 
  else {
    // if in range, calculate resistance value
    resistance = Re / ((Vref-Vm)/(volt-Vm) - 1);
  }
  return (resistance); 
}

// Rewrite these into two functions so can retrive the resistance
// separately and use temperature to get correct value;
float GetHumidity(float resistance, float temperature) {
  float humidity;
  
  if (resistance < 0) {
     // this means there was some error in the resistance measurement
     if (resistance == -1) {
       humidity = 0.0;
     } else if (resistance == -2) {
       humidity = 100.0; 
     } else {
        humidity = 0.0; 
     }
  }
  else {
    float reslog = log10(resistance);
    humidity = a * reslog*reslog + b * reslog + c;
    if (humidity > 100.0) {
      humidity = 100.0;
    } 
    else if (humidity < 0.0) {
      humidity = 0.0;
    }
  }
  return (humidity);
}

float GetTemperature(void) {
  float temperature;
  temperature = analogRead(tempPin) * 5.0 / 1024.0 / 0.01;
  return (temperature);
}

