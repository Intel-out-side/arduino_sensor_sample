#include <M5Stack.h>

#define motor_pin 26

int freq = 10000;
int ledChannel = 0;
int resolution = 10;
void setup() {
  M5.begin(); //Init M5Stack.  初始化M5Stack
  M5.Power.begin(); //Init power  初始化电源模块
  M5.lcd.setTextSize(2);  //Set the text size to 2.  设置文字大小为2
  M5.Lcd.setCursor(140, 10); //Set the cursor at (140,10).  将光标设置在(140,10)处
  M5.Lcd.println("Fan");
  
  ledcSetup(ledChannel, freq, resolution); 
  //Sets the frequency and number of counts corresponding to the channel.  设置通道对应的频率和计数位数
  
  ledcAttachPin(motor_pin, ledChannel);
  //Binds the specified channel to the specified I/O port for output.  将指定通道绑定到指定 IO 口上以实现输出
}

void loop() {
  for (int i=0; i<513; i+=100) {
      ledcWrite(ledChannel, i);
      delay(100);
    }
}
