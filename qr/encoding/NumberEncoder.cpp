#include "NumberEncoder.h"
#include "EncodingHelper.h"
#include "../utils/QrException.h"

using namespace qr::encoding;

std::string NumberEncoder::encode(char ch, bool flush) {
    // setting into buffer
    mCharacterBuffer[mBufferLen++] = ch - '0';
    if (mBufferLen == 3) {
        // we can use flush method because its exactly the same with 3 number
        flush = true;
    }
    // if the buffer is not full, but we need to flush (1 or 2 character)
    if (flush) {
        // encoding number
        uint number = 0;
        for (uint i = 0; i < mBufferLen; ++i) {
            number = number * 10 + mCharacterBuffer[i];
        }
        std::string binary;
        if (mBufferLen == 3) {
            binary = EncodingHelper::getBinaryOf(number, 10);
        } else {
            // we should set 
            binary = EncodingHelper::getBinaryOf(number);
        }
        mBufferLen = 0;

        // no need for fix width bitpattern
        return EncodingHelper::getBinaryOf(number, 10, true);
    }
    // otherwise giving back empty string
    return "";
}

std::string NumberEncoder::getCharacterCount(int count, int version) {
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
        // 10 bits long
        totalLength = 10;
    } else if (version < 27) {
        // 12 bits long
        totalLength = 12;
    } else if (version < 41) {
        // 14 bits long
        totalLength = 14;
    } else {
        throw QRException("Wrong version! Version must be smaller than 40.");
    }

    // filling buffer with reverse bit-pattern of count
    return EncodingHelper::getBinaryOf(count, totalLength, true);
}

std::string NumberEncoder::getModeIndicator() {
    // numberic mode's indicator: 0001
    return "0001";
}

std::string NumberEncoder::getEncoderName() {
    return "Number";
}

uint NumberEncoder::getVersion(uint length, ErrorCorrectionLevel EClevel) {
    /*
     * A table for holding the length of message for
     * × every version between 1 and 40
     * × for every EClevel
     */
    constexpr uint table[160] = {
        41, 34, 27, 17, 77, 63, 48, 34, 127, 101, 77, 58, 187, 149, 111, 82, 255,
        202, 144, 106, 322, 255, 178, 139, 370, 293, 207, 154, 461, 365, 259, 202,
        552, 432, 312, 235, 652, 513, 364, 288, 772, 604, 427, 331, 883, 691, 489,
        374, 1022, 796, 580, 427, 1101, 871, 621, 468, 1250, 991, 703, 530, 1408,
        1082, 775, 602, 1548, 1212, 876, 674, 1725, 1346, 948, 746, 1903, 1500,
        1063, 813, 2061, 1600, 1159, 919, 2232, 1708, 1224, 969, 2409, 1872, 1358,
        1056, 2620, 2059, 1468, 1108, 2812, 2188, 1588, 1228, 3057, 2395, 1718,
        1286, 3283, 2544, 1804, 1425, 3517, 2701, 1933, 1501, 3669, 2857, 2085,
        1581, 3909, 3035, 2181, 1677, 4158, 3289, 2358, 1782, 4417, 3486, 2473,
        1897, 4686, 3693, 2670, 2022, 4965, 3909, 2805, 2157, 5253, 4134, 2949,
        2301, 5529, 4343, 3081, 2361, 5836, 4588, 3244, 2524, 6153, 4775, 3417,
        2625, 6479, 5039, 3599, 2735, 6743, 5313, 3791, 2927, 7089, 5596, 3993, 3057
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

NumberEncoder::~NumberEncoder() {

}






