#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>

class Util
{
    public:
        byte* F2B(float f) 
        {
            unsigned int asInt = *((int*)&f);

            int i;
            for (i = 0; i < 4; i++) {
                floatBuf[i] = (asInt >> 8 * i) & 0xFF;
            }

            return floatBuf;
        }
    private:
        byte floatBuf[4];
};

#endif