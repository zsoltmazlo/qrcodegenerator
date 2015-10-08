

#include <algorithm>
#include <iosfwd>

#include "ErrorCorrection.h"

using namespace qr::correction;

int CorrectionHelper::searchInAntilogTable(int N, LogType type) {
    constexpr int logs[512] = {
                               1, 0, 2, 0, 4, 1, 8, 25, 16, 2, 32, 50, 64, 26, 128, 198,
                               29, 3, 58, 223, 116, 51, 232, 238, 205, 27, 135, 104, 19, 199, 38, 75,
                               76, 4, 152, 100, 45, 224, 90, 14, 180, 52, 117, 141, 234, 239, 201, 129,
                               143, 28, 3, 193, 6, 105, 12, 248, 24, 200, 48, 8, 96, 76, 192, 113,
                               157, 5, 39, 138, 78, 101, 156, 47, 37, 225, 74, 36, 148, 15, 53, 33,
                               106, 53, 212, 147, 181, 142, 119, 218, 238, 240, 193, 18, 159, 130, 35, 69,
                               70, 29, 140, 181, 5, 194, 10, 125, 20, 106, 40, 39, 80, 249, 160, 185,
                               93, 201, 186, 154, 105, 9, 210, 120, 185, 77, 111, 228, 222, 114, 161, 166,
                               95, 6, 190, 191, 97, 139, 194, 98, 153, 102, 47, 221, 94, 48, 188, 253,
                               101, 226, 202, 152, 137, 37, 15, 179, 30, 16, 60, 145, 120, 34, 240, 136,
                               253, 54, 231, 208, 211, 148, 187, 206, 107, 143, 214, 150, 177, 219, 127, 189,
                               254, 241, 225, 210, 223, 19, 163, 92, 91, 131, 182, 56, 113, 70, 226, 64,
                               217, 30, 175, 66, 67, 182, 134, 163, 17, 195, 34, 72, 68, 126, 136, 110,
                               13, 107, 26, 58, 52, 40, 104, 84, 208, 250, 189, 133, 103, 186, 206, 61,
                               129, 202, 31, 94, 62, 155, 124, 159, 248, 10, 237, 21, 199, 121, 147, 43,
                               59, 78, 118, 212, 236, 229, 197, 172, 151, 115, 51, 243, 102, 167, 204, 87,
                               133, 7, 23, 112, 46, 192, 92, 247, 184, 140, 109, 128, 218, 99, 169, 13,
                               79, 103, 158, 74, 33, 222, 66, 237, 132, 49, 21, 197, 42, 254, 84, 24,
                               168, 227, 77, 165, 154, 153, 41, 119, 82, 38, 164, 184, 85, 180, 170, 124,
                               73, 17, 146, 68, 57, 146, 114, 217, 228, 35, 213, 32, 183, 137, 115, 46,
                               230, 55, 209, 63, 191, 209, 99, 91, 198, 149, 145, 188, 63, 207, 126, 205,
                               252, 144, 229, 135, 215, 151, 179, 178, 123, 220, 246, 252, 241, 190, 255, 97,
                               227, 242, 219, 86, 171, 211, 75, 171, 150, 20, 49, 42, 98, 93, 196, 158,
                               149, 132, 55, 60, 110, 57, 220, 83, 165, 71, 87, 109, 174, 65, 65, 162,
                               130, 31, 25, 45, 50, 67, 100, 216, 200, 183, 141, 123, 7, 164, 14, 118,
                               28, 196, 56, 23, 112, 73, 224, 236, 221, 127, 167, 12, 83, 111, 166, 246,
                               81, 108, 162, 161, 89, 59, 178, 82, 121, 41, 242, 157, 249, 85, 239, 170,
                               195, 251, 155, 96, 43, 134, 86, 177, 172, 187, 69, 204, 138, 62, 9, 90,
                               18, 203, 36, 89, 72, 95, 144, 176, 61, 156, 122, 169, 244, 160, 245, 81,
                               247, 11, 243, 245, 251, 22, 235, 235, 203, 122, 139, 117, 11, 44, 22, 215,
                               44, 79, 88, 174, 176, 213, 125, 233, 250, 230, 233, 231, 207, 173, 131, 232,
                               27, 116, 54, 214, 108, 244, 216, 234, 173, 168, 71, 80, 142, 88, 1, 175
    };
    return logs[N * 2 + (int) type];
}

std::vector<int> CorrectionHelper::getGeneratorPolynomialCoefficients(uint n) {
    switch (n) {
        case 7:

            return
        {
            0, 87, 229, 146, 149, 238, 102, 21
        };
            break;
        case 8:
            return
        {
            0, 175, 238, 208, 249, 215, 252, 196, 28
        };
            break;
        case 9:
            return
        {
            0, 95, 246, 137, 231, 235, 149, 11, 123, 36
        };
            break;
        case 10:
            return
        {
            0, 251, 67, 46, 61, 118, 70, 64, 94, 32, 45
        };
            break;
        case 11:
            return
        {
            0, 220, 192, 91, 194, 172, 177, 209, 116, 227, 10, 55
        };
            break;
        case 12:
            return
        {
            0, 102, 43, 98, 121, 187, 113, 198, 143, 131, 87, 157, 66
        };
            break;
        case 13:
            return
        {
            0, 74, 152, 176, 100, 86, 100, 106, 104, 130, 218, 206, 140, 78
        };
            break;
        case 14:
            return
        {
            0, 199, 249, 155, 48, 190, 124, 218, 137, 216, 87, 207, 59, 22, 91
        };
            break;
        case 15:
            return
        {
            0, 8, 183, 61, 91, 202, 37, 51, 58, 58, 237, 140, 124, 5, 99, 105
        };
            break;
        case 16:
            return
        {
            0, 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120
        };
            break;
        case 17:
            return
        {
            0, 43, 139, 206, 78, 43, 239, 123, 206, 214, 147, 24, 99, 150, 39, 243, 163, 136
        };
            break;
        case 18:
            return
        {
            0, 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153
        };
            break;
        case 19:
            return
        {
            0, 67, 3, 105, 153, 52, 90, 83, 17, 150, 159, 44, 128, 153, 133, 252, 222, 138, 220, 171
        };
            break;
        case 20:
            return
        {
            0, 17, 60, 79, 50, 61, 163, 26, 187, 202, 180, 221, 225, 83, 239, 156, 164, 212, 212, 188, 190
        };
            break;
        case 21:
            return
        {
            0, 240, 233, 104, 247, 181, 140, 67, 98, 85, 200, 210, 115, 148, 137, 230, 36, 122, 254, 148, 175, 210
        };
            break;
        case 22:
            return
        {
            0, 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105, 165, 231
        };
            break;
        case 23:
            return
        {
            0, 171, 102, 146, 91, 49, 103, 65, 17, 193, 150, 14, 25, 183, 248, 94, 164, 224, 192, 1, 78, 56, 147, 253
        };
            break;
        case 24:
            return
        {
            0, 229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218, 111, 0, 117, 232, 87, 96, 227, 21
        };
            break;
        case 25:
            return
        {
            0, 231, 181, 156, 39, 170, 26, 12, 59, 15, 148, 201, 54, 66, 237, 208, 99, 167, 144, 182, 95, 243, 129, 178, 252, 45
        };
            break;
        case 26:
            return
        {
            0, 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102, 48, 227, 153, 145, 218, 70
        };
            break;
        case 27:
            return
        {
            0, 79, 228, 8, 165, 227, 21, 180, 29, 9, 237, 70, 99, 45, 58, 138, 135, 73, 126, 172, 94, 216, 193, 157, 26, 17, 149, 96
        };
            break;
        case 28:
            return
        {
            0, 168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201, 21, 43, 245, 87, 42, 195, 212, 119, 242, 37, 9, 123
        };
            break;
        case 29:
            return
        {
            0, 156, 45, 183, 29, 151, 219, 54, 96, 249, 24, 136, 5, 241, 175, 189, 28, 75, 234, 150, 148, 23, 9, 202, 162, 68, 250, 140, 24, 151
        };
            break;
        case 30:
            return
        {
            0, 41, 173, 145, 152, 216, 31, 179, 182, 50, 48, 110, 86, 239, 96, 222, 125, 42, 173, 226, 193, 224, 130, 156, 37, 251, 216, 238, 40, 192, 180
        };
            break;
    }
}

std::vector<int> CorrectionHelper::getErrorCorrectionCodewords(std::vector<int> message, uint coefficientCount) {
    /**
     * The number of steps in the division must equal the number of terms in the
     * message polynomial. 
     * In this case, the division will take 16 steps to complete.
     */
    // generator polynomial együtthatói (a^n, n-et kapjuk)
    std::vector<int> gen = getGeneratorPolynomialCoefficients(coefficientCount);

    // creating 3 array for holding the whole equation's coefficients
    std::vector<int> msgPoly = std::vector<int>(message.size() + gen.size());
    std::vector<int> genPoly = std::vector<int>(message.size() + gen.size());
    std::vector<int> genPolyWorking = std::vector<int>(message.size() + gen.size());

    // copy data into several arrays
    std::copy(message.begin(), message.end(), msgPoly.begin());
    std::copy(gen.begin(), gen.end(), genPoly.begin());

    // polynomial multiplications
    for (uint step = 0; step < message.size(); step++) {
        // Step 1a: Multiply the Generator Polynomial by the Lead Term of the Message Polynomial
        int k = searchInAntilogTable(msgPoly[step], LogType::AntiLog);
        for (uint i = 0; i < coefficientCount + 1; i++) {
            genPolyWorking[i + step] = (genPoly[i] + k) % 255;
        }

        // Step 1b: XOR the result with the message polynomial
        for (uint i = step; i < coefficientCount + 1 + step; i++) {
            msgPoly[i] = msgPoly[i]^searchInAntilogTable(genPolyWorking[i], LogType::Log);
        }
    }

    // copy all non-zero codewords into the result array
    std::vector<int> errorCorrectionCodewords = std::vector<int>(coefficientCount);
    for (uint i = 0; i < coefficientCount; ++i) {
        errorCorrectionCodewords[i] = msgPoly[i + message.size()];
    }
    return errorCorrectionCodewords;
}

