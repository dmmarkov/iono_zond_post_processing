#include "crc32.hpp"

#include <vector>

unsigned int
NovatelCRC32Value(int i) {
    int j;
    unsigned int ulCRC = i;
    for (j = 8; j > 0; j--) {
        if (ulCRC & 1)
            ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
        else
            ulCRC >>= 1;
    }

    return ulCRC;
}

unsigned int
NovatelEvaluateCRC32Value(
                          std::size_t ulCount, /* Number of bytes in the data block */
                          unsigned char *ucBuffer) /* Data block */ {
    unsigned int ulCRC = 0;

    while (ulCount-- != 0) {
        unsigned int ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
        unsigned int ulTemp3 = *ucBuffer;
        unsigned int ulTemp2 = NovatelCRC32Value(((int) ulCRC ^ ulTemp3) & 0xff);
        ulCRC = ulTemp1 ^ ulTemp2;
        ucBuffer++;
    }

    return ulCRC;
}