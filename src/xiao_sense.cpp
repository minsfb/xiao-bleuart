#include "xiao_sense.h"
// #include "serial_printf.h"

XiaoSense::XiaoSense()
{
}

bool XiaoSense::Update()
{
    UpdateImu();

    if(IsConnected())
    {
        bleSerial.poll();
        SendUart();
        SendBattery();
    }
    
    // if(Serial)
    // {
    //     Serial_printf(Serial, "IMU: %3f,%3f,%3f, %3f, %3f, %3f\tBAT: %d\%", aX, aY, aZ, gX, gY, gZ, batteryLevel);
    // }

    if(!IsConnected())
    {
        delay(50);  // this will low-power so we arent wasting cycles with no ble connected
    }
}

bool XiaoSense::SetupImu()
{
    return myIMU.begin() != 0;
}

// this unrolls the HwBLE.beginAndSetupBLE, but adds battery and some other stuff
bool XiaoSense::SetupBLE(const char *name)
{
  if (!BLE.begin()) { return false; }

  BLE.setLocalName(name);
  BLE.setDeviceName(name);
  
  // uart setup
  bleSerial.begin();
  
  // battery setup
  bleBatteryService.addCharacteristic(batteryLevelChar);
  BLE.addService(bleBatteryService);
  batteryLevelChar.writeValue(batteryLevel);
  
  BLE.advertise();
  return true;
}

bool XiaoSense::UpdateImu()
{
  if(nextImuReadTime > millis()) { return false; }
  nextImuReadTime = millis() + imuUpdatePeriod;

  aX = myIMU.readFloatAccelX();
  aY = myIMU.readFloatAccelY();
  aZ = myIMU.readFloatAccelZ();
  gX = myIMU.readFloatGyroX();
  gY = myIMU.readFloatGyroY();
  gZ = myIMU.readFloatGyroZ();

  return true;
}

bool XiaoSense::SetupBattery()
{
  pinMode(PIN_VBAT, INPUT); //Battery Voltage monitoring pin

  pinMode(P0_13, OUTPUT);   //Charge Current setting pin
  pinMode(P0_14, OUTPUT);   //Enable Battery Voltage monitoring pin
  digitalWrite(P0_13, LOW); //Charge Current 100mA   
  digitalWrite(P0_14, LOW); //Enable

  analogReference(AR_VDD);  //Vref=3.3V
  analogReadResolution(12); //12bits
}

bool XiaoSense::UpdateBattery()
{
  int batRaw = analogRead(PIN_VBAT);

  // adjust for voltage diviver, adc ref voltage, adc resolution
  float batVoltage = ((510e3 + 1000e3) / 510e3) * 3.3 * batRaw / 4095;

  // map voltage to oversimple lipo range (rated 3.7V, min 3.2V, full at 4.2V)
  int newLevel = map(batVoltage, 3.2, 4.7, 0, 100);

  // if(abs(newLevel - batteryLevel) < 5) { return false; }
  
  batteryLevel = newLevel;
}

bool XiaoSense::SendUart()
{
  if(nextUartTime > millis()) { return false; }
  nextUartTime = millis() + uartTxPeriod;

  sprintf(bleUartOutBuf, "%f,%f,%f, %f, %f, %f\n", aX, aY, aZ, gX, gY, gZ);

  bleSerial.print(bleUartOutBuf);
}

bool XiaoSense::SendBattery()
{
  if(nextBatteryTime > millis()) { return false; }
  nextBatteryTime = millis() + batTxPeriod;

  UpdateBattery();
  batteryLevelChar.writeValue(batteryLevel); 
}

bool XiaoSense::Setup(const char *name)
{
    return SetupBattery() && SetupImu() && SetupBLE(name);
}

bool XiaoSense::IsConnected()
{
  return BLE.connected();
}