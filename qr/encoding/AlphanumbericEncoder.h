/* 
 * File:   AlphanumbericMode.h
 * Author: zsoltmazlo
 *
 * Created on April 27, 2015, 6:35 PM
 */

#pragma once

#include <string>
#include "IEncoder.h"

namespace qr {
    
    namespace encoding {
        
        class AlphanumbericEncoder : public IEncoder {
            int mCharacterBuffer[2];
            int mBufferLen = 0;

        public:

            std::string getEncoderName() override;

            std::string getCharacterCount(int count, int version) override;

            std::string encode(char ch, bool flush) override;

            std::string getModeIndicator() override;

            uint getVersion(uint length, ErrorCorrectionLevel EClevel) override;

            ~AlphanumbericEncoder();
        };
    }
}



