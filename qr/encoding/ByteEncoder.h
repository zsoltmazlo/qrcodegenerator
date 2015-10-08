/* 
 * File:   ByteEncoder.h
 * Author: zsoltmazlo
 *
 * Created on May 2, 2015, 10:37 AM
 */

#pragma once

#include <string>
#include "IEncoder.h"

namespace qr {
    
    namespace encoding {
        
        class ByteEncoder : public IEncoder {
        public:
            
            std::string getEncoderName() override;

            std::string getCharacterCount(int count, int version) override;

            std::string encode(char ch, bool flush) override;

            std::string getModeIndicator() override;

            uint getVersion(uint length, ErrorCorrectionLevel EClevel) override;

            ~ByteEncoder();

        };
    }
}


