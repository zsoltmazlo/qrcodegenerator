/* 
 * File:   QRCodeGenerator.h
 * Author: zsoltmazlo
 *
 * Created on April 21, 2015, 11:59 PM
 */


#pragma once

#include "correction/ErrorCorrection.h"
#include "utils/Configuration.h"

#include <vector>
#include <algorithm>
#include "visualization/IVisualizer.h"


#include "encoding/DataAnalizer.h"
#include "masking/Mask.h"
#include "utils/ByteArray.h"

#include "utils/LogFormatter.h"
#include "utils/BlockRange.h"


namespace qr {

    class QRCodeGenerator {
        std::shared_ptr<encoding::IEncoder> mEncoder;

        encoding::ErrorCorrectionLevel mEClevel;

        masking::Mask mMask = masking::Mask::ApplyBest;

        visualization::IVisualizer* mVizualizer = nullptr;

        uint mVersion;

        utils::Configuration mConfig;

        // storing all codewords
        std::vector<int> mDataCodewords;

        // storing all error correction codewords
        std::vector<int> mECCodewords;

        // storing the ranges of blocks
        std::vector<utils::BlockRange> mBlockRanges;

        uint mTotalBlockCount;

        std::string mMessage;

        std::string mRawMessage;

        std::string mFileName;

        static std::ostream* console;

        utils::LogFormatter formatter;

    public:

        QRCodeGenerator();

        QRCodeGenerator(std::ostream* stream);

        static std::ostream* getConsole();
        
        /**
         * Creates an ByteArray from string, which could be using to generate
         * an qr code
         * 
         * The ByteArray object is allocating dynamically, and the caller must
         * delete it later!
         *  
         * @param message message to display (could be utf8 text)
         * @return An ByteArray pointer to the data
         */
        static utils::ByteArray* fromString(std::string& message) {
            utils::ByteArray* arr = new utils::ByteArray();
            arr->reserve(message.size());
            uint i=0;
            for(char ch: message) {
                (*arr)[i++] = (uint)ch;
            }
            return arr;
        }

        QRCodeGenerator& operator<<(encoding::ErrorCorrectionLevel EClevel);

        QRCodeGenerator& operator<<(const masking::Mask mask);

        QRCodeGenerator& operator<<(visualization::IVisualizer* visualizer);

        QRCodeGenerator& operator<<(const std::string& message);

        QRCodeGenerator& operator<<(const utils::ByteArray& data);

        void generate();

        std::string getFileName();

        virtual ~QRCodeGenerator();

        friend
        std::ostream& operator<<(std::ostream& os, const QRCodeGenerator& generator);

    private:

        void calculateDataCodewords(const utils::ByteArray& data);

        uint calculateBlockRanges();

        void calculateErrorCorrectionCodewords();

        void interleaveCodewords(uint maxBlockCount);

        void createBitStream(const utils::ByteArray& data);
    };


}


