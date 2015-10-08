/* 
 * File:   ErrorCorrection.h
 * Author: zsoltmazlo
 *
 * Created on April 21, 2015, 11:26 PM
 */

#pragma once

#include <cstring>
#include <vector>

namespace qr {
    
    using uint = unsigned int;

    namespace correction {

        template<int N>
        struct GaloisField {
            static constexpr int value = (GaloisField<N - 1 > ::value * 2) > 255 ? (GaloisField<N - 1 > ::value * 2)^285 : GaloisField<N - 1 > ::value * 2;
        };

        template<>
        struct GaloisField<0> {
            static constexpr int value = 1;
        };

        enum LogType {
            Log = 0,
            AntiLog = 1
        };

        class CorrectionHelper {
        public:

            /**
             * The following table contains the log and antilog values that are used
             * in GF(256) arithmetic, which is used for generating the error correction
             * codes required for QR codes.
             * 
             * source: http://www.thonky.com/qr-code-tutorial/log-antilog-table/
             * 
             * @param N number between 0..256, which need to be found
             * @param type which type we want: Log or Antilog
             * @return the value of Log or Antilog of N
             */
            static int searchInAntilogTable(int N, LogType type);

            /**
             * Load generator polynomial coefficients into buffer depending on the 
             * number of error correction codewords
             * 
             * for exp. for 7 codewords:
             * α^0*x^7 + α^87*x^6 + α^229*x^5 + α^146*x^4 + α^149*x^3 + α^238*x^2 + α^102*x^1 + α^21*x^0
             * would give
             * [0, 87, 229, 146, 149, 238, 102, 21]
             * 
             * source: http://www.thonky.com/qr-code-tutorial/generator-polynomial-tool/
             * 
             * @param n number of error correction codewords
             * @return a vector filled with coefficients
             */
            static std::vector<int> getGeneratorPolynomialCoefficients(uint n);

            /**
             * calculation error correction codewords for message
             * 
             * implemented following http://www.thonky.com/qr-code-tutorial/error-correction-coding/ article
             * 
             * @param message the actual message in bytes
             * @param coefficientCount how many coefficient should generate
             * @return vector of the generated error correction codewords
             */
            static std::vector<int> getErrorCorrectionCodewords(std::vector<int> message, uint coefficientCount);

        };
    }
};



