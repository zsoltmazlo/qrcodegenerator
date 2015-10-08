#include "EncodingHelper.h"
#include "AlphanumbericEncoder.h"
#include "../utils/QrException.h"

using namespace qr::encoding;

std::string AlphanumbericEncoder::getEncoderName() {
    return "Alphanumberic";
}

std::string AlphanumbericEncoder::encode(char ch, bool flush) {
    uint number = 0, index = 0;
    if (EncodingHelper::isNumber(ch)) {
        number = ch - '0';
    } else if (EncodingHelper::isAlphabetic(ch)) {
        number = ch - 'A' + 10;
    } else if (EncodingHelper::isPunction(ch, &index)) {
        // checking if given character is a punctual mark
        number = index + 36;
    } else {
        throw QRException("Wrong version! Version must be smaller than 40.");
    }

    // setting into buffer
    mCharacterBuffer[mBufferLen++] = number;
    if (mBufferLen == 2) {
        // buffer is filled, we must encode it
        uint number = mCharacterBuffer[0]*45 + mCharacterBuffer[1];
        mBufferLen = 0;
        // encoding number
        return EncodingHelper::getBinaryOf(number, 11, true);
    }

    // if the buffer is not full, but we need to flush (1 character)
    // --> 6 bit representant
    if (flush) {
        // encoding number
        mBufferLen = 0;
        return EncodingHelper::getBinaryOf(number, 6, true);
    }
    // otherwise giving back empty string
    return "";
}

std::string AlphanumbericEncoder::getCharacterCount(int count, int version) {
    /**
     * Versions 1 through 9
     * Numeric mode: 10 bits
     * Alphanumeric mode: 9 bits
     * Byte mode: 8 bits
     * Japanese mode: 8 bits
     * 
     * Versions 10 through 26
     * Numeric mode: 12 bits
     * Alphanumeric mode: 11 bits
     * Byte mode: 16
     * Japanese mode: 10 bits
     * 
     * Versions 27 through 40
     * Numeric mode: 14 bits
     * Alphanumeric mode: 13 bits
     * Byte mode: 16 bits
     * Japanese mode: 12 bits
     */
    int totalLength = 0;
    if (version < 10) {
        // 9 bits long
        totalLength = 9;
    } else if (version < 27) {
        // 11 bits long
        totalLength = 11;
    } else if (version < 41) {
        // 13 bits long
        totalLength = 13;
    } else {
        throw QRException("Wrong version! Version must be smaller than 40.");
    }

    // filling buffer with reverse bit-pattern of count
    return EncodingHelper::getBinaryOf(count, totalLength, true);
}

std::string AlphanumbericEncoder::getModeIndicator() {
    // alphanumberic mode's indicator: 0010
    return "0010";
}

uint AlphanumbericEncoder::getVersion(uint length, ErrorCorrectionLevel EClevel) {
    /*
     * A table for holding the length of message for
     * × every version between 1 and 40
     * × for every EClevel
     */
    constexpr uint table[160] = {
                                 25, 20, 16, 10, 47, 38, 29, 20, 77, 61, 47, 35, 114, 90, 67,
                                 50, 154, 122, 87, 64, 195, 154, 108, 84, 224, 178, 125, 93,
                                 279, 221, 157, 122, 335, 262, 189, 143, 395, 311, 221, 174,
                                 468, 366, 259, 200, 535, 419, 296, 227, 619, 483, 352, 259,
                                 667, 528, 376, 283, 758, 600, 426, 321, 854, 656, 470, 365,
                                 938, 734, 531, 408, 1046, 816, 574, 452, 1153, 909, 644, 493,
                                 1249, 970, 702, 557, 1352, 1035, 742, 587, 1460, 1134, 823,
                                 640, 1588, 1248, 890, 672, 1704, 1326, 963, 744, 1853, 1451,
                                 1041, 779, 1990, 1542, 1094, 864, 2132, 1637, 1172, 910, 2223,
                                 1732, 1263, 958, 2369, 1839, 1322, 1016, 2520, 1994, 1429,
                                 1080, 2677, 2113, 1499, 1150, 2840, 2238, 1618, 1226, 3009,
                                 2369, 1700, 1307, 3183, 2506, 1787, 1394, 3351, 2632, 1867,
                                 1431, 3537, 2780, 1966, 1530, 3729, 2894, 2071, 1591, 3927,
                                 3054, 2181, 1658, 4087, 3220, 2298, 1774, 4296, 3391, 2420, 1852,
    };

    // every 4th element is the next version with the same EC level
    // so k*4+Q is version k with Q Error Correction level
    // so convert ECLevel into an int
    uint i = (uint) EClevel;
    for (; i < 160; i += 4) {
        if (table[i] > length) {
            return i / 4 + 1;
        }
    }
    return 0;
}

AlphanumbericEncoder::~AlphanumbericEncoder() {

}

