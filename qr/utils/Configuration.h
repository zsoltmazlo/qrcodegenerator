/* 
 * File:   Configuration.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 3:24 PM
 */

#pragma once

#include "qr/encoding/IEncoder.h"

#include <iostream>

namespace qr {

    namespace utils {

        struct Group {
            /**
             * Number of Blocks in Group
             */
            uint blockCount;

            /**
             * Number of Data Codewords in Each of Group's Blocks
             */
            uint dataCodewordsInBlockCount;
        };

        class Configuration {
        public:
            /**
             * EC Codewords Per Block
             * 
             * the number of Error Correction codewords in each block
             */
            uint ECCodewordsCount;

            /**
             * Total Number of Data Codewords for this Version and EC Level
             */
            uint totalCodewordsCount;

            /*
             * holds the number of how many remainder bits need to concatenate to
             * the result bitstring (depending on version)
             */
            uint remainderBits;

            Group groups[2];

            static Configuration getConfiguration(uint version, encoding::ErrorCorrectionLevel EClevel);
        };

        std::ostream& operator<<(std::ostream& os, const Configuration& config);
    }
}

