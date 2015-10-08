
#include "IMaskPattern.h"
#include "qr/QRCodeGenerator.h"

using namespace qr;

masking::IMaskPattern::IMaskPattern(utils::ModuleMatrix matrix) {
    mMatrix = matrix;
}

void masking::IMaskPattern::apply(uint maskNumber) {
    
    mMatrix.buildFormatString(maskNumber);
    
    uint width = mMatrix.getDimension().width;
    uint height = mMatrix.getDimension().height;
    for (uint row = 0; row < height; ++row) {
        for (uint col = 0; col < width; ++col) {
            // Do not mask function patterns (finder patterns, timing patterns, separators, alignment patterns)
            if (mMatrix.isModuleReserved(col, row)) {
                continue;
            }
            if ((*this)(row, col)) {
                mMatrix(row, col) = mMatrix(row, col) == 1 ? 0 : 1;
            }
        }
    }
}

uint masking::IMaskPattern::evaulate() {
    uint ev1 = evaulateCondition1();
    uint ev2 = evaulateCondition2();
    uint ev3 = evaulateCondition3();
    uint ev4 = evaulateCondition4();
    std::ostream* console = QRCodeGenerator::getConsole();
    if (console != nullptr) {
        (*console) << "  cond#1: " << ev1 << std::endl
                << "  cond#2: " << ev2 << std::endl
                << "  cond#3: " << ev3 << std::endl
                << "  cond#4: " << ev4 << std::endl;
    }
    return ev1 + ev2 + ev3 + ev4;
}

uint masking::IMaskPattern::evaulateCondition1() {
    /*
     * Evaluation Condition #1
     * 
     * For the first evaluation condition, check each row one-by-one. If there 
     * are five consecutive modules of the same color, add 3 to the penalty. If
     * there are more modules of the same color after the first five, add 1 for 
     * each additional module of the same color. Afterward, check each column 
     * one-by-one, checking for the same condition. Add the horizontal and 
     * vertical total to obtain penalty score #1.
     */
    uint range = 0;
    uint penalty = 0;

    // working with shorter variable names makes it much easier to read :)
    uint height = mMatrix.getDimension().height;
    uint width = mMatrix.getDimension().width;

    // calculate rows
    for (uint row = 0; row < height; ++row) {
        // we could start at the #2 module to check the for a range
        range = 1;
        for (uint col = 1; col < width; ++col) {

            if (mMatrix(row, col) == mMatrix(row, col - 1)) {
                // if there is a pattern in modules
                ++range;
            } else {
                // if the pattern is ended
                if (range > 4) {
                    penalty += (range - 2);
                }
                // a new pattern started
                range = 1;
            }
        }
        // if we are at the end of the column we check it again
        if (range > 4) {
            penalty += (range - 2);
        }
    }

    // calculate columns
    for (uint col = 0; col < width; ++col) {
        // we could start at the #2 module to check the range
        range = 1;
        for (uint row = 1; row < height; ++row) {

            if (mMatrix(row, col) == mMatrix(row - 1, col)) {
                // if there is a pattern in modules, makes range higher
                ++range;
            } else {
                // if the pattern is ended
                if (range > 4) {
                    penalty += (range - 2);
                }
                // a new pattern started
                range = 1;
            }
        }
        // if we are at the end of the row we check it again
        if (range > 4) {
            penalty += (range - 2);
        }
    }

    return penalty;
}

uint masking::IMaskPattern::evaulateCondition2() {
    /*
     * Evaluation Condition #2
     * 
     * For second evaluation condition, look for areas of the same color that are
     * at least 2x2 modules or larger. The QR code specification says that for a
     * solid-color block of size m × n, the penalty score is 3 × (m - 1) × (n - 1).
     * However, the QR code specification does not specify how to calculate the 
     * penalty when there are multiple ways of dividing up the solid-color blocks.
     * 
     * Therefore, rather than looking for solid-color blocks larger than 2x2, 
     * simply add 3 to the penalty score for every 2x2 block of the same color 
     * in the QR code, making sure to count overlapping 2x2 blocks. For example,
     * a 3x2 block of the same color should be counted as two 2x2 blocks, one 
     * overlapping the other.
     * 
     * The following image illustrates how to calculate penalty rule #2.
     */
    uint penalty = 0;
    for (uint row = 0; row < mMatrix.getDimension().height - 1; ++row) {
        for (uint col = 0; col < mMatrix.getDimension().width - 1; ++col) {
            uint matcher = mMatrix(row, col)
                    + mMatrix(row, col + 1)
                    + mMatrix(row + 1, col)
                    + mMatrix(row + 1, col + 1);

            // if all of them are 1 or 0, then the values only could be 4 or 0
            if (matcher == 4 || matcher == 0) {
                penalty += 3;
            }
        }
    }
    return penalty;
}

uint masking::IMaskPattern::evaulateCondition3() {
    /*
     * Evaluation Condition #3
     * 
     * The third penalty rule looks for patterns of 
     * dark-light-dark-dark-dark-light-dark that have four light modules on 
     * either side. In other words, it looks for any of the following two 
     * patterns:
     * 
     * 10111010000 = 1488
     * 00001011101 = 93
     * 
     * Each time this pattern is found, add 40 to the penalty score.
     * 
     * Solution:
     * from the beginning of each row, starting to mainpulate a bitpattern from 
     * zero. Every bit from the mask shifted into it, and calculate the decimal
     * value of it.
     * 
     * Example:
     * row: 00001000010111010001
     * m:
     * 00000000000 = 0,
     * 00000000000 = 0,
     * 00000000000 = 0,
     * 00000000000 = 0,
     * 00000000001 = 1,
     * 00000000010 = 2,
     * 00000000100 = 4,
     * 00000001000 = 8,
     * 00000100001 = 33,
     * 00001000010 = 66,
     * 00010000101 = 133,
     * 00100001011 = 267,
     * 01000010111 = 535,
     * 10000101110 = 1070,
     *100001011101 = would be 2141, but last 11 bit is 93 = match!,
     * 00010111010 = 186,
     * 00101110100 = 372,
     * 01011101000 = 744,
     * 10111010001 = 1489,
     * and so on...
     */
    constexpr uint pattern1 = 1488;
    constexpr uint pattern2 = 93;
    constexpr uint last11bit = 2047;
    uint penalty = 0;

    // for each row
    for (uint row = 0; row < mMatrix.getDimension().height; ++row) {
        uint matcher = last11bit;
        for (uint col = 0; col < mMatrix.getDimension().width; ++col) {
            // shifting matcher pattern and setting the last bit the current
            // column's value
            matcher = ((matcher << 1) | mMatrix(row, col)) & last11bit;
            if (matcher == pattern1 || matcher == pattern2) {
                penalty += 40;
            }
        }
    }

    // for each column
    for (uint col = 0; col < mMatrix.getDimension().width; ++col) {
        uint matcher = last11bit;
        for (uint row = 0; row < mMatrix.getDimension().height; ++row) {
            // shifting matcher pattern and setting the last bit the current
            // column's value
            matcher = ((matcher << 1) | mMatrix(row, col)) & last11bit;
            if (matcher == pattern1 || matcher == pattern2) {
                penalty += 40;
            }
        }
    }


    return penalty;
}

uint masking::IMaskPattern::evaulateCondition4() {
    /*
     * Evaluation Condition #4
     * 
     * The final evaluation condition is based on the ratio of light modules to 
     * dark modules. To calculate this penalty rule, do the following steps:
     * 
     *  1, Count the total number of modules in the matrix.
     *  2, Count how many dark modules there are in the matrix.
     *  3, Calculate the percent of modules in the matrix that are dark: 
     *      (darkmodules / totalmodules) * 100
     *  4, Determine the previous and next multiple of five of this percent. 
     *      For example, for 43 percent, the previous multiple of five is 40, 
     *      and the next multiple of five is 45.
     *  5, Subtract 50 from each of these multiples of five and take the absolute 
     *      value of the result. For example, |40 - 50| = |-10| = 10 and |45 - 50| = |-5| = 5.
     *  6, Divide each of these by five. For example, 10/5 = 2 and 5/5 = 1.
     *  7, Finally, take the smallest of the two numbers and multiply it by 10. 
     *      In this example, the lower number is 1, so the result is 10. 
     *      This is penalty score #4.
     */
    uint height = mMatrix.getDimension().height;
    uint width = mMatrix.getDimension().width;

    // storing all dark modules
    uint dark = 0;
    for (uint row = 0; row < height; ++row) {
        for (uint col = 0; col < width; ++col) {
            // because we store only 0s and 1s in matrix, if it's a dark module,
            // then the counter increments - otherwise doing nothing
            // no need for branches :)
            dark += mMatrix(row, col);
        }
    }
    // calculating percentages (floor)
    int percent = (int) ((float) dark / (float) (width * height) * 100);
    int prev = percent - (percent % 5);
    int next = prev + 5;
    float pen1 = abs(prev - 50) / 5.0;
    float pen2 = abs(next - 50) / 5.0;
    return pen1 < pen2 ? (int)(pen1*10) : (int)(pen2*10);
}

utils::ModuleMatrix& masking::IMaskPattern::getMatrix() {
    return mMatrix;
}

bool masking::IMaskPattern::operator()(uint, uint) {
    return false;
}










