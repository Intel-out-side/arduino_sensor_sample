#include <Wire.h>


//#define DRV8830_WRITE_ADDR 0xC8
//#define DRV8830_READ_ADDR 0xC9
//#define CONTROL 0x00
//#define FAULT 0x01

const byte DRV8830_WRITE_ADDR = 0xC8>>1;
const byte DRV8830_READ_ADDR  = 0xC9>>1;
const byte CONTROL            = 0x00>>1;
const byte FAULT              = 0x01>>1;

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

  Serial.println("initialization clear");

  delay(1000);
}

void loop(){

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

  delay(10000);

  // warmup 2
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(0x0C<<2|0x02);
  Wire.endTransmission();

  delay(10000);

  // teikaku
  Wire.beginTransmission(DRV8830_WRITE_ADDR);
  Wire.write(CONTROL);
  Wire.write(0x12<<2|0x02);
  Wire.endTransmission();


  Serial.println("reached");
  while(1);
}
