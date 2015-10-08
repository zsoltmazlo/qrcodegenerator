/* 
 * File:   NumberEncoder.h
 * Author: zsoltmazlo
 *
 * Created on April 28, 2015, 6:51 PM
 */

#pragma once


#include <string>
#include "IEncoder.h"

namespace qr {
    
    namespace encoding {
        
        class NumberEncoder : public IEncoder {
            int mCharacterBuffer[3];
            uint mBufferLen = 0;

        public:
            

            std::string getEncoderName() override;

            std::string getCharacterCount(int count, int version) override;

            std::string encode(char ch, bool flush) override;

            std::string getModeIndicator() override;

            uint getVersion(uint length, ErrorCorrectionLevel EClevel) override;

            ~NumberEncoder();

        };
    }
}




