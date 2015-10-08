#include "EncodingHelper.h"

#include <sstream>
#include <algorithm>

using namespace qr::encoding;

std::string EncodingHelper::getBinaryOf(uint number, int length, bool needToReverse) {
    std::stringstream buffer;
    int currentLength = 0;
    // filling buffer with reverse bit-pattern of count
    while (number > 0) {
        buffer << (char) ((number % 2) + '0');
        // shifting count
        number >>= 1;
        ++currentLength;
    }
    // if string need to be exacty n size, then adding padding 0-s to it
    while (currentLength < length) {
        buffer << '0';
        ++currentLength;
    }
    // reversing bit-pattern if necessary 
    std::string reversed = buffer.str();
    if (needToReverse) {
        std::reverse(reversed.begin(), reversed.end());
    }
    return reversed;
}

int EncodingHelper::valueOf(std::string stringRepresentant) {
    int value = 0;
    for (auto ch : stringRepresentant) {
        // shift one to left (*2)
        value <<= 1;
        value += (int) (ch - '0');
    }
    return value;
}

bool EncodingHelper::isAlphabetic(char character, bool onlyUppercase) {
    bool lowercase, uppercase;
    lowercase = (character >= 'a' && character <= 'z');
    uppercase = (character >= 'A' && character <= 'Z');
    return (lowercase && !onlyUppercase) || uppercase;
}

bool EncodingHelper::isNumber(char character) {
    return (character >= '0' && character <= '9');
}

bool EncodingHelper::isPunction(char character, uint* indexHolder) {
    constexpr char punctuals[] = {' ', '$', '%', '*', '+', '-', '.', '/', ':'};
    uint i = 0;
    for (; i < 9; ++i) {
        if (punctuals[i] == character) {
            if (indexHolder != nullptr) {
                // for later usage we could save index of matching
                *indexHolder = i;
            }
            break;
        }
    }
    // i is lower than 9, then the character were in the array
    return i != 9;
}





