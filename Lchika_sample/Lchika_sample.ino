const int X_OUT = A2;
const int Y_OUT = A1;
const int Z_OUT = A0;
const int ANALOG_MIN = 0;
const int ANALOG_MAX = 1024; // 3.3VがMAXの場合
const double ZEROG_OFFSET = 1600.0;

double getAcc(double voltage) {
  double acc;
  acc = 3.0 + (voltage - (ZEROG_OFFSET + 900.0)) / 300.0;

  if (acc > 3.0) {
    return 3.0;
  }
  else if (acc < -3.0) {
    return -3.0;
  }
  return acc;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int xRaw, yRaw, zRaw;
  long xMill, yMill, zMill; // milli-g変換後の出力
  double x, y, z;
  double volt_x, volt_y, volt_z;

  xRaw = analogRead(X_OUT);
  yRaw = analogRead(Y_OUT);
  zRaw = analogRead(Z_OUT);

  volt_x = (double)(5.0 * xRaw) / ANALOG_MAX * 1000; //mV
  volt_y = (double)(5.0 * yRaw) / ANALOG_MAX * 1000;
  volt_z = (double)(5.0 * zRaw) / ANALOG_MAX * 1000;

  x = getAcc(volt_x);
  y = getAcc(volt_y);
  z = getAcc(volt_z);

  Serial.println(x);

}
