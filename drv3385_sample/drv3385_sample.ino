#include <Wire.h>


//#define DRV8830_WRITE_ADDR 0xC8
//#define DRV8830_READ_ADDR 0xC9
//#define CONTROL 0x00
//#define FAULT 0x01

const byte DRV8830_WRITE_ADDR = 0xC6>>1;
const byte DRV8830_READ_ADDR  = 0xC7>>1;
const byte CONTROL            = 0x00;
const byte FAULT              = 0x01;

const int ANALOG_MIN = 0;
const int ANALOG_MAX = 1023;
const int analogPin = A0;

int rawVal;
float voltage;
int motorVoltage;

void setup() {
  Serial.begin(19200);
  Wire.begin();

  // DRV8830 CONTROL clear
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(0x00<<2 | 0x00); //control clear. Write 0x0000
  byte error = Wire.endTransmission(true);

  // DRV8830 FAULT clear
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(FAULT);
  Wire.write(0x80);
  Wire.endTransmission();

  // Serial.println("initialization clear");
  Serial.println("voltage, motoroutput");

  //standing by
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(0x00<<2|0x00);
  Wire.endTransmission();

  delay(10);

  // warmup 1
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(0x06<<2|0x02);
  Wire.endTransmission();
}

int voltToMotorOut(float volt) {
  int output = (int)(19.0/3.46)*volt;
  return output;
}

void loop(){
  rawVal = analogRead(analogPin);
  voltage = 5.0 * rawVal / ANALOG_MAX;

  motorVoltage = voltToMotorOut(voltage);

  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(motorVoltage<<2 | 0x02);
  Wire.endTransmission();
  Serial.print(voltage);
  Serial.print(",");
  Serial.println(motorVoltage);
  delay(20);
}
