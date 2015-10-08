#include "ByteEncoder.h"
#include "EncodingHelper.h"
#include "../utils/QrException.h"

using namespace qr::encoding;

std::string ByteEncoder::encode(char ch, bool) {
    // cating to uchar which could represent as a binary
    unsigned char character = (unsigned char)ch;
    return EncodingHelper::getBinaryOf(character, 8, true);
}

std::string ByteEncoder::getCharacterCount(int count, int version) {
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
        totalLength = 8;
    } else if (version < 27) {
        totalLength = 16;
    } else if (version < 41) {
        totalLength = 16;
    } else {
        throw QRException("Wrong version! Version must be smaller than 40.");
    }
    
    // filling buffer with reverse bit-pattern of count
    return EncodingHelper::getBinaryOf(count, totalLength, true);;
}

std::string ByteEncoder::getEncoderName() {
    return "Byte";
}

std::string ByteEncoder::getModeIndicator() {
    return "0100";
}

uint ByteEncoder::getVersion(uint length, ErrorCorrectionLevel EClevel) {
    /*
     * A table for holding the length of message for
     * × every version between 1 and 40
     * × for every EClevel
     */
    constexpr uint table[160] = {
        17, 14, 11, 7, 32, 26, 20, 14, 53, 42, 32, 24, 78, 62, 46,
        34, 106, 84, 60, 44, 134, 106, 74, 58, 154, 122, 86, 64, 192,
        152, 108, 84, 230, 180, 130, 98, 271, 213, 151, 119, 321, 251,
        177, 137, 367, 287, 203, 155, 425, 331, 241, 177, 458, 362,
        258, 194, 520, 412, 292, 220, 586, 450, 322, 250, 644, 504,
        364, 280, 718, 560, 394, 310, 792, 624, 442, 338, 858, 666,
        482, 382, 929, 711, 509, 403, 1003, 779, 565, 439, 1091, 857,
        611, 461, 1171, 911, 661, 511, 1273, 997, 715, 535, 1367, 1059,
        751, 593, 1465, 1125, 805, 625, 1528, 1190, 868, 658, 1628,
        1264, 908, 698, 1732, 1370, 982, 742, 1840, 1452, 1030, 790,
        1952, 1538, 1112, 842, 2068, 1628, 1168, 898, 2188, 1722,
        1228, 958, 2303, 1809, 1283, 983, 2431, 1911, 1351, 1051,
        2563, 1989, 1423, 1093, 2699, 2099, 1499, 1139, 2809, 2213,
        1579, 1219, 2953, 2331, 1663, 1273
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

ByteEncoder::~ByteEncoder() {

}






