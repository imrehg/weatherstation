#include <limits.h>  // For ULONG_MAX

static const int VCCPin = 9;  // Digital, Power pin to drive the 1kHz square wave
static const int humidityPin = 0;  // Analog, Humidity sensor reading pin
static const int tempPin = 4;  // Analog, Temperature sensor reading pin

static const float Re = 50.3;  // Reference resistor R1, kOhm, measured
static const float Vref = 5.0;  // Voltage high reference (Vcc high)
static const float Vm = 2.5;  // Voltage middle reference (capacitors are charged to this on average)

// Fitted parameters for Resitance->Humidity conversion of CM-R
// at 25C
static float a = 2.76366367;
static float b = -32.40924966;
static float c = 102.93566686;

// Helper variables
int val = 0;
int count = 0;
unsigned long t;
boolean level = LOW;
unsigned long nowt;
static unsigned long microwait = 500;  // Half length of 1kHz period in microsecond
unsigned long deltat;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(VCCPin, OUTPUT);
  t = micros();
}

void loop() {
  float temperature, humidity, resistance;

  // Get the elapsed time
  nowt = micros();
  if (nowt >= t) {
    deltat = nowt - t;
  } 
  else {
    // Taking care of micros overflow that should happen every 70 minutes or so
    deltat = ULONG_MAX - t + nowt;
  }

  // If half a period is elapsed, switch the output level of VCC to get the square wave
  if (deltat > microwait) {
    digitalWrite(VCCPin, level);
    if (level == HIGH) {
      // After every this many switching, do a weather reading
      // count set empirically to approx. 1 times every second (Leonardo)
      if (count == 935) {

        // Read resistor value
        resistance = GetResistance();

        digitalWrite(VCCPin, LOW);
        delay(50);
        temperature = GetTemperature();
        digitalWrite(VCCPin, HIGH);

        // Do the conversion from resistance->humidity
        humidity = GetHumidity(resistance);

        // Send data over the serial connection
        Serial.print("H,");  // Device identifier message header
        Serial.print(humidity);
        Serial.print(",");
        Serial.print(resistance);
        Serial.print(",");
        Serial.print(temperature);
        Serial.println();
        count = 0;  // restart counter
      } else {
        count++;  // count up till have to measure values again
      }
    }
    level = !level;  // switch the levels
    t = nowt;
  }  
}

// Output resistance in kOhm
float GetResistance(void) {
  float resistance, voltage;
  int val;
  
  val = analogRead(humidityPin);
  voltage = val * 5.0 / 1024;

  if (voltage < Vm) {
    resistance = -1;
  } 
  else if (voltage > Vref) {
    resistance = -2;
  } 
  else {
    // if in range, calculate resistance value
    resistance = Re / ((Vref-Vm)/(voltage-Vm) - 1);
  }
  return (resistance); 
}

// Convert resistance value into humidity value using the polinomial parameters
float GetHumidity(float resistance) {
  float humidity;
  
  if (resistance <= 0) {
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
    // positive resistance reading
    float reslog = log10(resistance);
    humidity = a * reslog*reslog + b * reslog + c;

    // set limits correctly
    if (humidity > 100.0) {
      humidity = 100.0;
    } 
    else if (humidity < 0.0) {
      humidity = 0.0;
    }

  }
  return (humidity);
}

// Temperature reading from voltage and LM335 conversion factor
float GetTemperature(void) {
  float temperature, voltage;
  voltage = analogRead(tempPin) * 5.0 / 1024.0;
  // LM335 has 10mV/K conversion
  temperature = voltage / 0.01;
  return (temperature);
}

