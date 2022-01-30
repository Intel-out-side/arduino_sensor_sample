#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  Serial.begin(19200);
  while (!Serial);

  Serial.println("intialized");

}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  int nDevices;

  nDevices = 0;

  for (address=1; address<127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4) {
      
    }
  }

  delay(5000);
}
