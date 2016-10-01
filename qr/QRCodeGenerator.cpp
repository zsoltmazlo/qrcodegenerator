#include "QRCodeGenerator.h"
#include "encoding/EncodingHelper.h"
#include "utils/ModuleMatrix.h"
#include "utils/QrException.h"
#include "utils/BlockRange.h"

#include <sstream>
#include <iterator>

using namespace qr;

std::ostream* QRCodeGenerator::console = nullptr;

QRCodeGenerator::QRCodeGenerator() : mEncoder(nullptr) {
    // if we not get any stream for output then we write everything to the 
    // standard output
    console = &(std::cout);
    
    // if not setted, we using the 'ApplyBest' mask
    mMask = masking::Mask::ApplyBest;
    
    // if error correction is not setted, than we use Q
    mEClevel = encoding::ErrorCorrectionLevel::Q;
}

std::ostream* QRCodeGenerator::getConsole() {
    return console;
}

QRCodeGenerator::QRCodeGenerator(std::ostream* stream) : mEncoder(nullptr) {
    console = stream;
    formatter.setConsole(console);
}

uint QRCodeGenerator::calculateBlockRanges() {
    // storing total count of blocks
    mTotalBlockCount = mConfig.groups[0].blockCount + mConfig.groups[1].blockCount;

    // creating groups and blocks
    // there would be 2 groups and each of them has constant number of blocks
    // we store the range of codewords for each block
    mBlockRanges = std::vector<utils::BlockRange>(mTotalBlockCount);

    uint start = 0, stop = 0, blockIndex = 0, maxBlockCount = 0;
    for (uint group = 0; group < 2; ++group) {
        uint blocksCount = mConfig.groups[group].blockCount;
        uint codewordsCount = mConfig.groups[group].dataCodewordsInBlockCount;
        if (codewordsCount > maxBlockCount) {
            maxBlockCount = codewordsCount;
        }
        // for each block...
        for (uint block = 0; block < blocksCount; ++block) {
            // storing start and stop indexes
            start = stop;
            stop = start + codewordsCount;
            mBlockRanges[blockIndex].start = start;
            mBlockRanges[blockIndex].stop = stop;
            ++blockIndex;
        }
    }

    return maxBlockCount;
}

void QRCodeGenerator::calculateDataCodewords(const utils::ByteArray& data) {
    // storing all bits in a buffer
    std::stringstream mBuffer;

    encoding::DataAnalizer analizer;
    utils::ByteArray& mData = const_cast<utils::ByteArray&> (data);
    analizer.analize(mData);
    mEncoder = analizer.getEncoder();


    // calculating version 
    mVersion = mEncoder->getVersion(mData.length(), mEClevel);

    /*
     * We know should to get the configuration of the QR
     */
    mConfig = utils::Configuration::getConfiguration(mVersion, mEClevel);

    if (console != nullptr) {
        (*console) << (*this);
    }
    std::string modeIndicator = mEncoder->getModeIndicator();
    std::string charCountIndicator = mEncoder->getCharacterCount(mData.length(), mVersion);

    // saving mode and charCount to buffer
    mBuffer << modeIndicator << charCountIndicator;

    for (unsigned int i = 0; i < mData.length(); ++i) {
        // if we at the last character we need the encoded letter(s) always! (flush)
        mBuffer << mEncoder->encode(mData[i], (i == mData.length() - 1));
    }

    /**
     * terminator zeros:
     * 
     * total=mConfig.totalCodewordsCount*8
     * 
     * if we are with 8 character or less before reaching total,
     * then we only add that many zeros
     * 
     * otherwise add zeros to reach length which divided by 8 is 0, and thats all
     */
    uint total = mConfig.totalCodewordsCount * 8;
    std::string msg = mBuffer.str();

    // if the message is almost filled the space up, than only set zeros
    if (msg.length() > total - 8) {
        uint zeros = total - msg.length();
        msg += encoding::EncodingHelper::getBinaryOf(1 << zeros, zeros, true);
    } else {
        // otherwise we just padding it to make it dividable with 8
        uint zeros = 8 - (msg.length() % 8);
        if (zeros < 8) {
            std::string padding = encoding::EncodingHelper::getBinaryOf(1 << zeros);
            msg += padding.substr(1, padding.length() - 1);
        }
    }

    // breaking buffer into codewords
//    mDataCodewords = new int[mConfig.totalCodewordsCount];
    mDataCodewords = std::vector<int>(mConfig.totalCodewordsCount);
    uint j = 0;
    for (uint i = 0; i < msg.length(); i += 8) {
        std::string cw = msg.substr(i, 8);
        // we can immediately encode the binarys
        mDataCodewords[j++] = encoding::EncodingHelper::valueOf(cw);
    }

    // filling up data codewords
    // padding bytes for fill out qr space PONTOSAN
    unsigned int mod = 0;
    constexpr unsigned int mPaddings[2] = {236, 17};
    while (j < mConfig.totalCodewordsCount) {
        mDataCodewords[j++] = mPaddings[mod++ % 2];
    }

    // for debug 
    if (console == nullptr) {
        return;
    }

    formatter.reset();
    *console << std::endl << std::endl << "data: " << std::endl;
    for (uint i = 0; i < mConfig.totalCodewordsCount; ++i) {
        // formatting output
        formatter();
        *console << encoding::EncodingHelper::getBinaryOf(mDataCodewords[i], 8) << " ";
    }
    *console << std::endl;

    formatter.reset().setWidth(9);
    for (uint i = 0; i < mConfig.totalCodewordsCount; ++i) {
        // formatting output
        formatter();
        *console << std::left << mDataCodewords[i];

    }
    *console << std::endl;
}

void QRCodeGenerator::calculateErrorCorrectionCodewords() {
    // storing total count of blocks
    mTotalBlockCount = mConfig.groups[0].blockCount + mConfig.groups[1].blockCount;

    // creating array for codewords
    mECCodewords = std::vector<int>(mTotalBlockCount * mConfig.ECCodewordsCount);
    

    uint blockIndex = 0, i;
    for (uint group = 0; group < 2; ++group) {
        uint blocksCount = mConfig.groups[group].blockCount;
        uint codewordsCount = mConfig.groups[group].dataCodewordsInBlockCount;

        // for each block...
        for (uint block = 0; block < blocksCount; ++block) {
            // storing start and stop indexes
            uint start = mBlockRanges[blockIndex].start;
            uint stop = mBlockRanges[blockIndex].stop;

            // block's codewords
            std::vector<int> dataCodewordsOfBlock = std::vector<int>(codewordsCount);
            std::copy(std::next(mDataCodewords.begin(), start), std::next(mDataCodewords.begin(), stop), dataCodewordsOfBlock.begin());
            std::vector<int> ECcodewordsOfBlock = qr::correction::CorrectionHelper::getErrorCorrectionCodewords(dataCodewordsOfBlock, mConfig.ECCodewordsCount);

            for (i = 0; i < mConfig.ECCodewordsCount; ++i) {
                mECCodewords[blockIndex * mConfig.ECCodewordsCount + i] = ECcodewordsOfBlock[i];
            }

            // ER calculating
            blockIndex++;
        }
    }

    // for debug 
    if (console == nullptr) {
        return;
    }


    formatter.reset();
    *console << std::endl << std::endl << "error correction: " << std::endl;
    for (uint i = 0; i < mTotalBlockCount * mConfig.ECCodewordsCount; ++i) {
        formatter();
        *console << encoding::EncodingHelper::getBinaryOf(mECCodewords[i], 8) << " ";
    }
    formatter.reset().setWidth(9);
    *console << std::endl;
    for (uint i = 0; i < mTotalBlockCount * mConfig.ECCodewordsCount; ++i) {
        formatter();
        *console << std::left << mECCodewords[i];
    }
    *console << std::endl;
}

void QRCodeGenerator::generate() {
    if( mMessage.length() == 0 ) {
        throw QRException("Message not setted!");
    }

    if (mVizualizer == nullptr) {
        throw QRException("No vizualizer setted!");
        return;
    }

    // creating a Module matrix from the given data
    utils::ModuleMatrix matrix;
    matrix.setQRVersion(mVersion);
    matrix.setMessage(mMessage);
    matrix.setErrorCorrectionLevel(mEClevel);
    matrix.setMask(mMask);
    matrix.build();

    // setting up visualizer
    mVizualizer->setModuleDimension(10);
    mVizualizer->setQRVersion(mVersion);

    // visualize it
    mVizualizer->visualize(matrix);
}

QRCodeGenerator& QRCodeGenerator::operator<<(encoding::ErrorCorrectionLevel EClevel) {
    mEClevel = EClevel;
    return *this;
}

QRCodeGenerator& QRCodeGenerator::operator<<(const masking::Mask mask) {
    mMask = mask;
    return *this;
}

QRCodeGenerator& QRCodeGenerator::operator<<(const std::string& message) {
    mRawMessage = message;
    
    // creating an ByteArray from data
    utils::ByteArray* arr = QRCodeGenerator::fromString(mRawMessage);
    createBitStream(*arr);
    delete arr;
    
    return *this;
}

QRCodeGenerator& QRCodeGenerator::operator<<(const utils::ByteArray& data) {
    createBitStream(data);
    return *this;
}

void QRCodeGenerator::createBitStream(const utils::ByteArray& data) {

    // 1. calculate all data codewords
    calculateDataCodewords(data);

    // 2. calculate block ranges
    uint maxBlockCount = calculateBlockRanges();

    // 3. calculate all error correction codewords
    calculateErrorCorrectionCodewords();

    // 4. interleaving data codewords
    // 5. interleaving error correction codewords
    interleaveCodewords(maxBlockCount);

    // 6. full message concatenation
    // because we need to encode all the codewords into binary right after
    // concatenation, there is no need to concat these two array
    // 7. convert all of them to binary string
    std::stringstream mBuffer;
    for (uint i = 0; i < mConfig.totalCodewordsCount; ++i) {
        mBuffer << encoding::EncodingHelper::getBinaryOf(mDataCodewords[i], 8, true);
    }
    for (uint i = 0; i < mConfig.ECCodewordsCount * mTotalBlockCount; ++i) {
        mBuffer << encoding::EncodingHelper::getBinaryOf(mECCodewords[i], 8, true);
    }

    // 8. remainder bits
    // we decodes 0 (which gives us only zero-s in )
    mBuffer << encoding::EncodingHelper::getBinaryOf(0, mConfig.remainderBits, false);

    // storing the message for visualization
    mMessage = mBuffer.str();
}

void QRCodeGenerator::interleaveCodewords(uint maxBlockCount) {
    /*
     * Please note that smaller QR codes only consist of one block of 
     * data codewords, with one set of error correction codewords for 
     * that block. In this case, no interleaving is necessary. 
     * Simply place the error correction codewords after the data 
     * codewords and move on to the next step, module placement in matrix.
     */
    if (mConfig.groups[1].blockCount == 0 && mConfig.groups[0].blockCount == 1) {
        return;
    }
    if (console != nullptr) {
        *console << "interleaved" << std::endl;
    }

    // 4. interleaving data codewords
    int cwIDX = 0;
    std::vector<int> dataCWsInterleaved = std::vector<int>(mConfig.totalCodewordsCount);
    for (uint i = 0; i < maxBlockCount; ++i) {
        for (uint j = 0; j < mTotalBlockCount; ++j) {
            // start + i
            uint idx = mBlockRanges[j].start + i;
            // if we over indexed the block
            if (mBlockRanges[j].stop > idx) {
                dataCWsInterleaved[cwIDX++] = mDataCodewords[idx];
            }
        }
    }

    // 5. interleaving error correction codewords
    cwIDX = 0;
    std::vector<int> ecCWsInterleaved = std::vector<int>(mConfig.ECCodewordsCount * mTotalBlockCount);
    for (uint i = 0; i < mConfig.ECCodewordsCount; ++i) {
        for (uint j = 0; j < mTotalBlockCount; ++j) {
            uint idx = i + j * mConfig.ECCodewordsCount;
            ecCWsInterleaved[cwIDX++] = mECCodewords[idx];
        }
    }

    // setting up for new codewords pointers for the interleaved ones
    mDataCodewords = dataCWsInterleaved;
    mECCodewords = ecCWsInterleaved;
}

QRCodeGenerator::~QRCodeGenerator() {
    delete mVizualizer;
}

QRCodeGenerator& QRCodeGenerator::operator<<(visualization::IVisualizer* visualizer) {
    mVizualizer = visualizer;
    // saving filename for later usage
    mFileName = visualizer->getFileName();
    return *this;
}

std::string QRCodeGenerator::getFileName() {
    return mFileName;
}

namespace qr {

    std::ostream& operator<<(std::ostream& os, const QRCodeGenerator& generator) {

        os << "qr generator settings: " << std::endl
                << "version.........: " << generator.mVersion << std::endl
                << "error correction: ";
        switch (generator.mEClevel) {
            case encoding::L:
                os << "L" << std::endl;
                break;
            case encoding::M:
                os << "M" << std::endl;
                break;
            case encoding::Q:
                os << "Q" << std::endl;
                break;
            case encoding::H:
                os << "H" << std::endl;
                break;
        }
        os << "message:........: '" << generator.mRawMessage << "'" << std::endl;
        os << "encoding mode...: " << generator.mEncoder->getEncoderName() << std::endl;
        os << std::endl << generator.mConfig;
        return os;
    }
}
