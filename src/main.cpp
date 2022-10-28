#include <LSM6DS3.h>
#include <stdarg.h>
#include <HardwareBLESerial.h>

#define BLENAME                       "MattSense"
#define SAMPLING_RATE                 50   //Hz
#define DURATION                      1 //seconds

// IMU
// https://github.com/Seeed-Studio/Seeed_Arduino_LSM6DS3/blob/master/LSM6DS3.cpp
// by default: 416Hz sample, max 16G and 2000deg/s
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
float aX, aY, aZ, gX, gY, gZ;

HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();

char outBuf[160];
void updateImuVals();

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!bleSerial.beginAndSetupBLE(BLENAME)) {
    while (true) {
      Serial.println("failed to initialize HardwareBLESerial!");
      delay(1000);
    }
  }

  // // wait for a central device to connect
  // while (!bleSerial);

  // Serial.println("HardwareBLESerial central device connected!");

  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("aX,aY,aZ,gX,gY,gZ");
  }
}

float lastUpdated = 0;

void loop() 
{
  updateImuVals();
  sprintf(outBuf, "%f,%f,%f, %f, %f, %f\n", aX, aY, aZ, gX, gY, gZ);

  if(bleSerial)
  {
    bleSerial.poll();
    bleSerial.print(outBuf);
  }
  
  if(Serial && (millis() - lastUpdated) > 1000)
  {
    lastUpdated = millis();
    Serial.print(outBuf);
  }

  if(!bleSerial)
  {
    delay(50);  // this will low-power so we arent wasting cycles with no ble connected
  }
}

void updateImuVals()
{
  aX = myIMU.readFloatAccelX();
  aY = myIMU.readFloatAccelY();
  aZ = myIMU.readFloatAccelZ();
  gX = myIMU.readFloatGyroX();
  gY = myIMU.readFloatGyroY();
  gZ = myIMU.readFloatGyroZ();
}

byte floatBuf[4];
byte* floatToByteArray(float f) 
{
    unsigned int asInt = *((int*)&f);

    int i;
    for (i = 0; i < 4; i++) {
        floatBuf[i] = (asInt >> 8 * i) & 0xFF;
    }

    return floatBuf;
}