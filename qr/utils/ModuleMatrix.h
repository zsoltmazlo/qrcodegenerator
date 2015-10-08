/* 
 * File:   ModuleMatrix.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 10:59 AM
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "qr/encoding/IEncoder.h"
#include "qr/masking/Mask.h"

namespace qr {

    namespace utils {

        class Rectangle {
        public:
            uint width;
            uint height;
        };

        class Point {
        public:
            uint x;
            uint y;

            Point(uint _x, uint _y) : x(_x), y(_y) { }
        };

        class ModuleMatrix {
            uint* matrix = nullptr;

            uint mVersion;

            Rectangle mDimension;

            std::string mMessage;

            encoding::ErrorCorrectionLevel mECLevel;

            masking::Mask mMask;

            std::vector<Point> alignments;

        public:

            ModuleMatrix();

            ModuleMatrix(const ModuleMatrix& orig);

            ModuleMatrix& operator=(const ModuleMatrix& orig);

            void setQRVersion(uint version);

            Rectangle getDimension() const;

            uint operator()(uint row, uint col) const;

            uint& operator()(uint row, uint col);

            void setMessage(std::string& message);

            void setErrorCorrectionLevel(encoding::ErrorCorrectionLevel eclevel);

            void setMask(masking::Mask mask);

            void build();

            friend
            std::ostream& operator<<(std::ostream& os, const qr::utils::ModuleMatrix& matrix);

            bool isModuleReserved(uint col, uint row) const;

            void buildFormatString(uint maskNumber);

            virtual ~ModuleMatrix();

            /**
             * gives back the alignment patterns' position depending on qr version
             * 
             * @param version the version of qr code
             * @param positions an array of 7 int for holding positions. if the position is 
             * not used then its 0
             */
            static void getAligmentPatternPosition(unsigned int version, unsigned int* positions);

        private:

            enum DisplayDirection {
                Upward_1stColumn,
                Upward_2ndColumn,
                Downward_1stColumn,
                Downward_2ndColumn
            };


            bool isNeedToReverse(uint col, uint row);

            void buildFinderPattern(uint row, uint col);

            void buildAligmentPatterns();

            void buildTimingPatterns();

            void buildMessagePattern();

            char* generateFormatStringV1_40(uint maskNumber);

            char* generateFormatStringV7_40();

            uint applyBestMask();
        };
    }

}


