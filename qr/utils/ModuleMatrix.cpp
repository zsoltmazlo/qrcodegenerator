#include <iosfwd>

#include "ModuleMatrix.h"
#include "qr/masking/masks.h"
#include "qr/masking/Masker.h"
#include "qr/visualization/IVisualizer.h"
#include "qr/QRCodeGenerator.h"

using namespace qr::utils;

void ModuleMatrix::setMask(masking::Mask mask) {
    mMask = mask;
}

void ModuleMatrix::build() {
    // #1 finder patterns
    buildFinderPattern(0, 0);
    buildFinderPattern(0, mDimension.width - 7);
    buildFinderPattern(mDimension.height - 7, 0);

    // #2 aligment patterns
    buildAligmentPatterns();

    // #3 timing pattern
    buildTimingPatterns();

    // #4 dark module
    (*this)(4 * mVersion + 9, 8) = 1;

    // #5 message modules
    buildMessagePattern();

    std::ostream* console = QRCodeGenerator::getConsole();
    if (console != nullptr) {
        (*console) << std::endl << "original message:" << std::endl;
        (*console) << *this << std::endl;
    }

    // masking
    applyBestMask();

}

void ModuleMatrix::buildFormatString(uint maskNumber) {
    // generating format string and finalizing qr code (its always needed!)
    char* formatString = generateFormatStringV1_40(maskNumber);

    // 14. bit --> below top right finder
    uint positions[15][4] = {
        {8, 0, mDimension.height - 1, 8}, // 14
        {8, 1, mDimension.height - 2, 8}, // 13
        {8, 2, mDimension.height - 3, 8}, // 12
        {8, 3, mDimension.height - 4, 8}, // 11
        {8, 4, mDimension.height - 5, 8}, // 10
        {8, 5, mDimension.height - 6, 8}, // 9
        {8, 7, mDimension.height - 7, 8}, // 8
        {8, 8, 8, mDimension.width - 8}, // 7
        {7, 8, 8, mDimension.width - 7}, // 6
        {5, 8, 8, mDimension.width - 6}, // 5
        {4, 8, 8, mDimension.width - 5}, // 4
        {3, 8, 8, mDimension.width - 4}, // 3
        {2, 8, 8, mDimension.width - 3}, // 2
        {1, 8, 8, mDimension.width - 2}, // 1
        {0, 8, 8, mDimension.width - 1} // 0
    };

    for (uint i = 0; i < 15; ++i) {
        uint val = formatString[i] - '0';
        (*this)(positions[i][0], positions[i][1]) = val;
        (*this)(positions[i][2], positions[i][3]) = val;
    }

    // for 7+ version and higher
    if (mVersion < 7) {
        return;
    }

    formatString = generateFormatStringV7_40();
    std::cout << formatString << std::endl;

    /*
     * Top Right Version Information Block
     * 00 01 02
     * 03 04 05
     * 06 07 08
     * 09 10 11
     * 12 13 14
     * 15 16 17 = formatString's first character
     * 
     * position:
     * x x  x 0 1 1 1 1 1 1 1 - top right corner (0, dimension.width-1)
     * x x  x 0 1 0 0 0 0 0 1
     * x x  x 0 1 0 1 1 1 0 1
     * x x  x 0 1 0 1 1 1 0 1
     * x x  x 0 1 0 1 1 1 0 1
     * x x 17 0 1 0 0 0 0 0 1  17: (5, dimension.width-9)
     *        0 1 1 1 1 1 1 1
     * 
     * Bottom Left Version Information Block
     * 00 03 06 09 12 15      
     * 01 04 07 10 13 16      
     * 02 05 08 11 14 17
     * 
     * x x x x x x
     * x x x x x x
     * x x x x x 17            17: (dimension.height-9, 5)
     * 0 0 0 0 0 0 0
     * 1 1 1 1 1 1 1
     * 1 0 0 0 0 0 1
     * 1 0 1 1 1 0 1
     * 1 0 1 1 1 0 1
     * 1 0 1 1 1 0 1
     * 1 0 0 0 0 0 1
     * 1 1 1 1 1 1 1
     * |
     * bottom left corner (dimension.height-1, 0)
     */
    uint row = 5, col = mDimension.width - 9; // using width==height equality
    for (uint i = 0; i < 18; ++i) {
        uint val = formatString[i] - '0';
        std::cout << row << ", " << col << "=" << val << std::endl;

        // top right
        (*this)(row, col) = val;

        // bottom left
        (*this)(col, row) = val;

        // going to right
        --col;
        if (col == mDimension.width - 12) {
            col = mDimension.width - 9;
            --row;
        }
    }
}

// type of an masker function (definition)
using maskerFunction = bool (*)(uint, uint);

uint ModuleMatrix::applyBestMask() {
    /*
     * The QR code specification defines eight mask patterns that can be applied 
     * to the QR code. For example, for mask pattern #1, every even-numbered row 
     * in the QR matrix is masked, and for mask pattern #2, every third column in
     * the QR matrix is masked.
     */

    maskerFunction functions[8] = {
                                   [](uint row, uint col) {
                                       return (row + col) % 2 == 0;
                                   },
                                   [](uint row, uint) {
                                       return (row) % 2 == 0;
                                   },
                                   [](uint, uint col) {
                                       return (col) % 3 == 0;
                                   },
                                   [](uint row, uint col) {
                                       return (row + col) % 3 == 0;
                                   },
                                   [](uint row, uint col) {
                                       return (row / 2 + col / 3) % 2 == 0;
                                   },
                                   [](uint row, uint col) {
                                       return ((row * col) % 2 + (row * col) % 3) == 0;
                                   },
                                   [](uint row, uint col) {
                                       return ( ((row * col) % 2) + ((row * col) % 3)) % 2 == 0;
                                   },
                                   [](uint row, uint col) {
                                       return ( ((row + col) % 2) + ((row * col) % 3)) % 2 == 0;
                                   },
    };

    masking::IMaskPattern * patterns[8];
    for (uint i = 0; i < 8; ++i) {
        patterns[i] = new masking::Masker(*this, functions[i]);
        patterns[i]->apply(i);
    }

    std::ostream* console = QRCodeGenerator::getConsole();

    // searching for the lowest
    // initialize the lowest value for UINT32 max
    // therefore every bit pattern penalty should be lower than this
    if (console != nullptr) {
        (*console) << std::endl << "masking" << std::endl;
    }

    uint lowest = UINT32_MAX, lowestIndex = 0;
    for (uint i = 0; i < 8; ++i) {
        if (console != nullptr) {
            (*console) << "mask" << i << std::endl;
            (*console) << patterns[i]->getMatrix() << std::endl;
        }
        uint penalty = patterns[i]->evaulate();
        if (console != nullptr) {
            (*console) << "   total: " << penalty << std::endl;
        }

        if (lowest >= penalty) {
            lowest = penalty;
            lowestIndex = i;
        }
    }

    // using setted mask
    switch (mMask) {
        case masking::Mask::ApplyBest:
            // dont do anything, the lowest number is good
            break;
        case masking::Mask::MaskNumber0:
        case masking::Mask::MaskNumber1:
        case masking::Mask::MaskNumber2:
        case masking::Mask::MaskNumber3:
        case masking::Mask::MaskNumber4:
        case masking::Mask::MaskNumber5:
        case masking::Mask::MaskNumber6:
        case masking::Mask::MaskNumber7:
            // overwrite mask number
            lowestIndex = (int) mMask;
            break;
    }

    if (console != nullptr) {
        (*console) << "best mask: " << lowestIndex << ", penalty: " << lowest << std::endl;
    }

    // copy all the element of the lowest mask
    ModuleMatrix bestMask = patterns[lowestIndex]->getMatrix();
    for (uint row = 0; row < mDimension.height; ++row) {
        for (uint col = 0; col < mDimension.width; ++col) {
            (*this)(row, col) = bestMask(row, col);
        }
    }

    // delete all pattern
    for (int i = 0; i < 8; ++i) {
        delete patterns[i];
    }

    // returning with the mask number which used (for calculating format string)
    return lowestIndex;
}

char* ModuleMatrix::generateFormatStringV1_40(uint maskNumber) {
    // http://www.thonky.com/qr-code-tutorial/format-version-tables/
    constexpr char* table[32] = {
                                 "111011111000100", // L,0
                                 "111001011110011", // L,1
                                 "111110110101010", // L,2
                                 "111100010011101", // L,3
                                 "110011000101111", // L,4
                                 "110001100011000", // L,5
                                 "110110001000001", // L,6
                                 "110100101110110", // L,7
                                 "101010000010010", // M,0
                                 "101000100100101", // M,1
                                 "101111001111100", // M,2
                                 "101101101001011", // M,3
                                 "100010111111001", // M,4
                                 "100000011001110", // M,5
                                 "100111110010111", // M,6
                                 "100101010100000", // M,7
                                 "011010101011111", // Q,0
                                 "011000001101000", // Q,1
                                 "011111100110001", // Q,2
                                 "011101000000110", // Q,3
                                 "010010010110100", // Q,4
                                 "010000110000011", // Q,5
                                 "010111011011010", // Q,6
                                 "010101111101101", // Q,7
                                 "001011010001001", // H,0
                                 "001001110111110", // H,1
                                 "001110011100111", // H,2
                                 "001100111010000", // H,3
                                 "000011101100010", // H,4
                                 "000001001010101", // H,5
                                 "000110100001100", // H,6
                                 "000100000111011" // H,7
    };
    return table[mECLevel * 8 + maskNumber];
}

char* ModuleMatrix::generateFormatStringV7_40() {
    constexpr char* table[34] = {
                                 "000111110010010100", // 7
                                 "001000010110111100", // 8
                                 "001001101010011001", // 9
                                 "001010010011010011", // 10
                                 "001011101111110110", // 11
                                 "001100011101100010", // 12
                                 "001101100001000111",
                                 "001110011000001101",
                                 "001111100100101000",
                                 "010000101101111000",
                                 "010001010001011101",
                                 "010010101000010111",
                                 "010011010100110010",
                                 "010100100110100110",
                                 "010101011010000011",
                                 "010110100011001001",
                                 "010111011111101100",
                                 "011000111011000100",
                                 "011001000111100001",
                                 "011010111110101011",
                                 "011011000010001110",
                                 "011100110000011010",
                                 "011101001100111111",
                                 "011110110101110101",
                                 "011111001001010000",
                                 "100000100111010101",
                                 "100001011011110000",
                                 "100010100010111010",
                                 "100011011110011111",
                                 "100100101100001011",
                                 "100101010000101110",
                                 "100110101001100100",
                                 "100111010101000001",
                                 "101000110001101001"
    };
    // we are absolutely sure that we get only 7+ version, because
    // the function is private and used only at one place
    return table[mVersion - 7];

}

// ezt itt hagyom, mert egy másfél órás munka után jöttem rá, hogy
// 1, ez a megoldás még mindig nem jó, 
// 2, a honlapon fent találhatóak a jó adatok
// de szerintem ez a kód itt nagyon aranyos :D

//typedef std::string(*getBinaryOf)(int, int, bool);
//typedef int (*valueOf)(std::string);
//void ModuleMatrix::generateFormatString(int maskNumber) {
// régi, és ráadásul még nem is jó
//    int format = 0;
//    std::cout << "level: " << mECLevel << ", mask: " << maskNumber << std::endl;
//    switch (mECLevel) {
//        case qr::encoding::L:
//            format = 1;
//            break;
//        case qr::encoding::M:
//            format = 0;
//            break;
//        case qr::encoding::Q:
//            format = 3;
//            break;
//        case qr::encoding::H:
//            format = 2;
//            break;
//    }
//    // shifting format with 3 bit
//    format <<= 3;
//    format += maskNumber;
//    
//    // generator polynom int representant (dont ask...)
//    int gen = 1335;
//
//    getBinaryOf binary = &encoding::EncodingHelper::getBinaryOf;
//    valueOf value = &encoding::EncodingHelper::valueOf;
//
//    // helper functions
//    auto trim = [ = ](std::string & msg){
//        int trimmed = (*value)(msg);
//        return (*binary)(trimmed, 0, true);
//    };
//
//    auto padding = [ = ](std::string& msg, int len){
//        int val = (*value)(msg);
//        if (len - msg.length() > 0) {
//            return (*binary)(val << (len - msg.length()), 0, true);
//        }
//        return (*binary)(val, 0, true);
//    };
//    
//    auto _xor = [=](std::string& m1, std::string& m2) {
//        int v1 = (*value)(m1);
//        int v2 = (*value)(m2);
//        return (*binary)(v1^v2, 0, true);
//    };
//
//
//    std::string formatString = (*binary)(format << 10, 15, true);
//    formatString = trim(formatString);
//    std::string genString = (*binary)(gen, 0, true);
//    
//    std::cout << "format string:" << formatString << std::endl;
//    std::cout << "gen string:   " << padding(genString, formatString.length()) << std::endl;
//
//    std::string xorred = formatString;
//    do {
//        
//        std::string _gen = padding(genString, xorred.length());
//        xorred = _xor(xorred, _gen);
//        xorred = trim(xorred);
//        
//        std::cout << "XOR:          " << xorred << "   (" <<  xorred.length() << ")" << std::endl;
//    } while (xorred.length() > 10);
//    
//    // padding left
//    xorred = (*binary)((*value)(xorred), 10, true);
//    std::cout << "error corr:   " << xorred << "   (" <<  xorred.length() << ")" << std::endl;
//    formatString = (*binary)(format, 5, true);
//    std::string mask = "101010000010010";
//    std::cout << "final:        " << formatString << xorred << std::endl;
//    std::cout << "mask:         " << mask << std::endl;
//    std::string asd = _xor(formatString, mask);
//    std::cout << "final:        " << asd << std::endl;
//}

void ModuleMatrix::buildAligmentPatterns() {
    // getting positions of 
    uint positions[7];
    ModuleMatrix::getAligmentPatternPosition(mVersion, positions);

    for (uint p1 = 0; p1 < 7; ++p1) {
        if (positions[p1] == 0) {
            continue;
        }
        for (uint p2 = 0; p2 < 7; ++p2) {
            if (positions[p2] == 0) {
                continue;
            }

            uint col = positions[p1];
            uint row = positions[p2];

            // aligment patterns must not overlap with finders!
            // top right
            if (col + 5 > mDimension.width - 8 && row < 8) {
                continue;
            }
            // top left
            if (col < 8 && row < 8) {
                continue;
            }
            // bottom left
            if (col < 8 && row + 5 > mDimension.height - 8) {
                continue;
            }

            // otherwise fill it with 1
            uint i = 0;
            for (; i < 5; ++i) {
                (*this)(row + i - 2, col - 2) = 1;
                (*this)(row - 2, col + i - 2) = 1;
            }
            for (i--; i > 0; --i) {
                (*this)(row + i - 2, col + 4 - 2) = 1;
                (*this)(row + 4 - 2, col + i - 2) = 1;
            }

            // middle module
            (*this)(row, col) = 1;

            // for later usage
            alignments.push_back(Point(col, row));
        }

    }
}

ModuleMatrix::ModuleMatrix() {
    mMask = masking::Mask::ApplyBest;
}

ModuleMatrix::ModuleMatrix(const ModuleMatrix& orig) {
    // copy of matrix
    matrix = new uint[orig.mDimension.height * orig.mDimension.width];
    for (uint i = 0; i < orig.mDimension.height * orig.mDimension.width; ++i) {
        matrix[i] = orig.matrix[i];
    }
    mDimension = orig.mDimension;
    mMessage = orig.mMessage;
    alignments = orig.alignments;
    mVersion = orig.mVersion;
    mECLevel = orig.mECLevel;

}

ModuleMatrix& ModuleMatrix::operator=(const ModuleMatrix& orig) {
    if (this != &orig) {
        if (matrix != nullptr) {
            delete[] matrix;
        }
        // copy of matrix
        matrix = new uint[orig.mDimension.height * orig.mDimension.width];
        for (uint i = 0; i < orig.mDimension.height * orig.mDimension.width; ++i) {
            matrix[i] = orig.matrix[i];
        }
        mDimension = orig.mDimension;
        mMessage = orig.mMessage;
        alignments = orig.alignments;
        mVersion = orig.mVersion;
        mECLevel = orig.mECLevel;
    }
    return *this;
}

void ModuleMatrix::buildFinderPattern(uint row, uint col) {
    // outer rectangle (stroke)
    uint i = 0, j = 0;
    for (; i < 7; ++i) {
        (*this)(row + i, col) = 1;
        (*this)(row, col + i) = 1;
    }
    for (i--; i > 0; --i) {
        (*this)(row + i, col + 6) = 1;
        (*this)(row + 6, col + i) = 1;
    }
    // inner rectangle (filled)
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            (*this)(row + i + 2, col + j + 2) = 1;
        }
    }
}

void ModuleMatrix::buildMessagePattern() {
    /**
     * The data bits are placed starting at the bottom-right of the 
     * matrix and proceeding upward in a column that is 2 modules wide.
     * When the column reaches the top, the next 2-module column starts 
     * immediately to the left of the previous column and continues 
     * downward. Whenever the current column reaches the edge of the 
     * matrix, move on to the next 2-module column and change direction. 
     * If a function pattern or reserved area is encountered, the data
     * bit is placed in the next unused module.
     */

    // reserve places
    Point origin = Point(mDimension.width - 1, mDimension.height - 1);
    DisplayDirection direction = Upward_1stColumn;
    uint moduleToDisplay = 0;
    while (true) {
        // we need an extra rule for vertical timing pattern:
        // we should set the whole stuff 1 module to the left
        if (origin.x == 6) {
            --origin.x;
        }

        if (!isModuleReserved(origin.x, origin.y)) {
            // getting next bit of message
            if (mMessage.length() < moduleToDisplay) {
                break;
            }
            char m = mMessage[moduleToDisplay++];
            (*this)(origin.y, origin.x) = m == '0' ? 0 : 1;
            //            (*this)(origin.y, origin.x) = m - '0';
        }


        // calculating next position
        switch (direction) {
            case Upward_1stColumn:
                origin.x -= 1;
                direction = Upward_2ndColumn;
                break;
            case Upward_2ndColumn:
                origin.x += 1;
                origin.y -= 1;
                direction = Upward_1stColumn;

                if (isNeedToReverse(origin.x, origin.y)) {
                    direction = Downward_1stColumn;
                    origin.x -= 2;
                    origin.y += 1;
                }
                break;
            case Downward_1stColumn:
                origin.x -= 1;
                direction = Downward_2ndColumn;
                break;
            case Downward_2ndColumn:
                direction = Downward_1stColumn;
                origin.x += 1;
                origin.y += 1;

                if (isNeedToReverse(origin.x, origin.y)) {
                    origin.y -= 1;
                    origin.x -= 2;
                    direction = Upward_1stColumn;
                }
                break;
        }
    }
}

void ModuleMatrix::buildTimingPatterns() {
    for (uint i = 8; i < mDimension.width - 8; i += 2) {
        (*this)(6, i) = 1;
        (*this)(i, 6) = 1;
    }
}

Rectangle ModuleMatrix::getDimension() const {
    return mDimension;
}

bool ModuleMatrix::isModuleReserved(uint col, uint row) const {
    // if x,y is in the top right finder && reserved territory
    if (col >= mDimension.width - 8 && row < 9) {
        return true;
    }
    // if x,y is in the top left finder && reserved territory
    if (col < 9 && row < 9) {
        return true;
    }
    // if x,y is in the bottom left finder && reserved territory
    if (col < 9 && row > mDimension.height - 9) {
        return true;
    }
    // if horizontal timing pattern
    if (row == 6) {
        return true;
    }
    // if vertical timing pattern
    if (col == 6) {
        return true;
    }

    // aligments
    for (auto point : alignments) {
        if (col >= point.x - 2 && col < point.x + 3) {
            if (row >= point.y - 2 && row < point.y + 3) {
                // we are in an aligment pattern!
                return true;
            }
        } else {
            continue;
        }
    }

    // if version is gt than 7, then two more area is reserved
    if (mVersion >= 7) {
        if ((col >= (mDimension.width - 11)) && row <= 5) {
            return true;
        }
        if (col <= 5 && (row >= (mDimension.height - 11))) {
            return true;
        }
    }

    // otherwise module could use to data
    return false;
}

bool ModuleMatrix::isNeedToReverse(uint col, uint row) {

    // if x,y is in the top right finder && reserved territory
    if (col >= mDimension.width - 7 && row < 9) {
        return true;
    }

    // bottom edge of qr
    if (row > mDimension.height - 1) {
        return true;
    }

    return false;
}

uint ModuleMatrix::operator()(uint row, uint col) const {
    return matrix[row * mDimension.width + col];
}

uint& ModuleMatrix::operator()(uint row, uint col) {
    return matrix[row * mDimension.width + col];
}

void ModuleMatrix::setMessage(std::string& message) {
    // make a copy of message!
    mMessage = message;
}

void ModuleMatrix::setErrorCorrectionLevel(encoding::ErrorCorrectionLevel eclevel) {
    mECLevel = eclevel;
}

void ModuleMatrix::setQRVersion(uint version) {
    mVersion = version;
    uint dim = ((version - 1)*4 + 21);
    // the matrix will hold 0 and 1-s for every module
    matrix = new uint[dim * dim];
    std::generate(matrix, matrix + dim*dim, [] {
        return 0;
    });

    // storing dimension
    mDimension.width = dim;
    mDimension.height = dim;
}

ModuleMatrix::~ModuleMatrix() {
    if (matrix != nullptr) {
        delete[] matrix;
    }
}

void ModuleMatrix::getAligmentPatternPosition(unsigned int version, unsigned int* positions) {
    if (version < 2) {
        for (unsigned int i = 0; i < 7; ++i) {
            positions[i] = 0;
        }
        return;
    }
    constexpr unsigned int table[39][7] = {
        {6, 18, 0, 0, 0, 0, 0},
        {6, 22, 0, 0, 0, 0, 0},
        {6, 26, 0, 0, 0, 0, 0},
        {6, 30, 0, 0, 0, 0, 0},
        {6, 34, 0, 0, 0, 0, 0},
        {6, 22, 38, 0, 0, 0, 0},
        {6, 24, 42, 0, 0, 0, 0},
        {6, 26, 46, 0, 0, 0, 0},
        {6, 28, 50, 0, 0, 0, 0},
        {6, 30, 54, 0, 0, 0, 0},
        {6, 32, 58, 0, 0, 0, 0},
        {6, 34, 62, 0, 0, 0, 0},
        {6, 26, 46, 66, 0, 0, 0},
        {6, 26, 48, 70, 0, 0, 0},
        {6, 26, 50, 74, 0, 0, 0},
        {6, 30, 54, 78, 0, 0, 0},
        {6, 30, 56, 82, 0, 0, 0},
        {6, 30, 58, 86, 0, 0, 0},
        {6, 34, 62, 90, 0, 0, 0},
        {6, 28, 50, 72, 94, 0, 0},
        {6, 26, 50, 74, 98, 0, 0},
        {6, 30, 54, 78, 102, 0, 0},
        {6, 28, 54, 80, 106, 0, 0},
        {6, 32, 58, 84, 110, 0, 0},
        {6, 30, 58, 86, 114, 0, 0},
        {6, 34, 62, 90, 118, 0, 0},
        {6, 26, 50, 74, 98, 122, 0},
        {6, 30, 54, 78, 102, 126, 0},
        {6, 26, 52, 78, 104, 130, 0},
        {6, 30, 56, 82, 108, 134, 0},
        {6, 34, 60, 86, 112, 138, 0},
        {6, 30, 58, 86, 114, 142, 0},
        {6, 34, 62, 90, 118, 146, 0},
        {6, 30, 54, 78, 102, 126, 150},
        {6, 24, 50, 76, 102, 128, 154},
        {6, 28, 54, 80, 106, 132, 158},
        {6, 32, 58, 84, 110, 136, 162},
        {6, 26, 54, 82, 110, 138, 166},
        {6, 30, 58, 86, 114, 142, 170}
    };
    for (unsigned int i = 0; i < 7; ++i) {
        positions[i] = table[version - 2][i];
    }
}


namespace qr {

    namespace utils {

        std::ostream& operator<<(std::ostream& os, const ModuleMatrix& matrix) {
            for (uint row = 0; row < matrix.getDimension().height; ++row) {
                for (uint col = 0; col < matrix.getDimension().width; ++col) {
                    os << matrix(row, col);
                }
                os << std::endl;
            }
            return os;
        }
    }
}