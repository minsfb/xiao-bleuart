#include "xiao_sense.h"
#include "serial_printf.h"

XiaoSense::XiaoSense()
{
}

bool XiaoSense::Update()
{
    UpdateBattery();
    UpdateImu();

    if(IsConnected())
    {
        bleSerial.poll();
        SendUart();
        SendBattery();
    }
    
    if(Serial)
    {
        Serial_printf(Serial, "\nIMU:\tx: %3f,\ty: %3f,\tz: %3f\pitch: %3f,\troll: %3f,\tyaw: %3f\tBAT: %d%%", aX, aY, aZ, gX, gY, gZ, batteryLevel);
    }

    if(!IsConnected())
    {
        delay(50);  // this will low-power so we arent wasting cycles with no ble connected
    }

    return true;
}

bool XiaoSense::SetupImu()
{
    myIMU = LSM6DS3(0u, 0x6A);
    return myIMU.begin() == 0;
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

  return true;
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

  return true;
}

bool XiaoSense::SendUart()
{
  if(nextUartTime > millis()) { return false; }
  nextUartTime = millis() + uartTxPeriod;

  sprintf(bleUartOutBuf, "%f,%f,%f,%f,%f,%f\n", aX, aY, aZ, gX, gY, gZ);

  bleSerial.print(bleUartOutBuf);

  return true;
}

bool XiaoSense::SendBattery()
{
  if(nextBatteryTime > millis()) { return false; }
  nextBatteryTime = millis() + batTxPeriod;

  UpdateBattery();
  batteryLevelChar.writeValue(batteryLevel); 
  
  return true;
}

bool XiaoSense::Setup(const char *name)
{
    bool batReady;
    Serial.print("Initing battery... ");
    do {
        batReady = SetupBattery();
        delay(1000);
        Serial.println(batReady ? "ready." : "failed.");
    } while(!batReady);

    bool bleReady;
    Serial.print( "Initing ble... ");
    do {
        bleReady = SetupBLE(name);
        delay(1000);
        Serial.println(bleReady ? "ready." : "failed.");
    } while(!bleReady);

    bool imuReady;
    Serial.print( "Initing imu... ");
    do {
        imuReady = SetupImu();
        delay(1000);
        Serial.println(imuReady ? "ready." : "failed.");
    } while(!imuReady);

    Serial.println("Xiao init complete.");

    return batReady && bleReady && imuReady;
}

bool XiaoSense::IsConnected()
{
  return BLE.connected();
}