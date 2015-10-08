/* 
 * File:   EncodingHelper.h
 * Author: zsoltmazlo
 *
 * Created on May 3, 2015, 9:28 PM
 */

#pragma once

#include <string>

namespace qr {

    namespace encoding {

        class EncodingHelper {
        public:

            static std::string getBinaryOf(uint number, int length = 0, bool needToReverse = true);

            static int valueOf(std::string stringRepresentant);

            static bool isNumber(char character);

            static bool isAlphabetic(char character, bool onlyUppercase = true);

            static bool isPunction(char character, uint* indexHolder = nullptr);
        };
    }
}



