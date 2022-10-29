#include <xiao_sense.h>
#include <LSM6DS3.h>
#include <Arduino.h>

#define BLENAME "MattSense"

XiaoSense &xiao = XiaoSense::Instance();

void setup() 
{
  Serial.begin(115200);

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