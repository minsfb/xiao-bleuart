#include <xiao_sense.h>

#define BLENAME "MattSense"

XiaoSense &xiao = XiaoSense::Instance();

void setup() {
  Serial.begin(115200);
  while (!Serial);
    Serial.println("Hi!");

  while (!xiao.Setup(BLENAME))
  {
    Serial.println("failed to initialize device!");
    delay(1000);
  }
}

void loop() 
{
  xiao.Update();
}