/* 
 * File:   encoding.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 11:30 AM
 */

#pragma once

#include <string>

namespace qr {

    namespace encoding {

        enum ErrorCorrectionLevel {
            L = 0,
            M = 1,
            Q = 2,
            H = 3
        };

        class IEncoder {
        protected:
            ErrorCorrectionLevel mECLevel = ErrorCorrectionLevel::L;

        public:

            /**
             * returing the name of encoding type
             * @return 
             */
            virtual std::string getEncoderName() = 0;

            /**
             * returning with the Encoding Mode's string representant
             * 
             * @return 
             */
            virtual std::string getModeIndicator() = 0;

            /**
             * calculate the binary representant of the message and returning
             * with it as a string
             * 
             * @param ch a character which should encode
             * @return 
             */
            virtual std::string encode(char ch, bool flush) = 0;

            /**
             * getting the version of the QRCode depending on Error Correction Level
             * and the length of the message
             * 
             * @param length length of the message
             * @return the version of the QRCode (a number between 1..40)
             */
            virtual uint getVersion(uint length, ErrorCorrectionLevel EClevel) = 0;

            /**
             * returns the string representant of the character counts
             * 
             * @param count how many characters we want to encode
             * @return 
             */
            virtual std::string getCharacterCount(int count, int version) = 0;

            virtual ~IEncoder() { }
        };
    }
}


