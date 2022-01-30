#define ANALOG_MIN 0
#define ANALOG_MAX 1023

const int sensorPin = A5;
const float potentio_max = 3.46;
const float potentio_min = 0.0;
float angle;
int rawSensorValue = 0; //Store the current value of the sensor

const int pwmPin = 5;


float getAngle(float voltage) {
  float angle = 1.0 / 3.46 * voltage;

  if (angle < 0) return 0;
  if (1.0 < angle) return 1.0;

  return angle;
}

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(pwmPin, OUTPUT);

  analogWrite(pwmPin, 100);
  Serial.begin(9600);
}


void loop() {
  float voltage;
  int i;
  rawSensorValue = analogRead(sensorPin);

  voltage = 5.0 * (float)rawSensorValue / ANALOG_MAX;

  angle = getAngle(voltage);

  i = (int)(angle * 255);

  analogWrite(pwmPin, i);

  
  Serial.println(angle);
  delay(10);
}
