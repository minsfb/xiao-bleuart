#ifndef __XIAO_SENSE_H__
#define __XIAO_SENSE_H__

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <HardwareBLESerial.h>

class XiaoSense
{
    public:
        // singleton instance
        static XiaoSense& Instance() {
            static XiaoSense instance; // instantiated on first use, guaranteed to be destroyed
            return instance;
        }

        bool Setup(const char *name);
        bool Update();
        bool Emit();
        bool IsConnected();

        operator bool();

    private:
        XiaoSense();
        XiaoSense(XiaoSense const &other) = delete;  // disable copy constructor
        void operator=(XiaoSense const &other) = delete;  // disable assign constructor

        // IMU
        // https://github.com/Seeed-Studio/Seeed_Arduino_LSM6DS3/blob/master/LSM6DS3.cpp
        // by default: 416Hz sample, max 16G and 2000deg/s
        LSM6DS3 myIMU = LSM6DS3(I2C_MODE, 0x6A);    //I2C device address 0x6A
        float aX, aY, aZ, gX, gY, gZ;
        int imuUpdatePeriod = 10;
        int nextImuReadTime;

        HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();
        int uartTxPeriod = 50;  // send uart updates every 50ms
        int nextUartTime = 0;
        char bleUartOutBuf[160];

        BLEService bleBatteryService = BLEService("180F");
        BLEUnsignedCharCharacteristic batteryLevelChar = BLEUnsignedCharCharacteristic("2A19",  BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes
        int batteryLevel = 0;  // last battery level reading from analog input
        long previousMillis = 0;  // last time the battery level was checked, in ms
        int batTxPeriod = 1000; // send battery update every 200ms
        int batUpdatePeriod = 200;
        int nextBatteryTime = 0;

        bool SetupImu();
        bool UpdateImu();
        bool SetupBLE(const char *name);

        bool SetupBattery();
        bool UpdateBattery();
        bool SendBattery();

        bool SendUart();
};

#endif