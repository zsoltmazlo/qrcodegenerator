/* 
 * File:   masking.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 10:57 AM
 */

#pragma once

#include "qr/utils/ModuleMatrix.h"
#include "IMaskPattern.h"

namespace qr {

    namespace masking {

        class MaskNumber0 : public IMaskPattern {
        public:
            
            // creating default constructor exactly the same definition
            // with the ancenstor class 
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return (row + col) % 2 == 0;
            }

        };

        class MaskNumber1 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint) override {
                return (row) % 2 == 0;
            }
        };

        class MaskNumber2 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint, uint col) override {
                return (col) % 3 == 0;
            }
        };

        class MaskNumber3 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return (row + col) % 3 == 0;
            }
        };

        class MaskNumber4 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return (row / 2 + col / 3) % 2 == 0;
            }
        };

        class MaskNumber5 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return ((row * col) % 2 + (row * col) % 3) == 0;
            }
        };

        class MaskNumber6 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return ( ((row * col) % 2) + ((row * col) % 3)) % 2 == 0;
            }
        };

        class MaskNumber7 : public IMaskPattern {
        public:
            using IMaskPattern::IMaskPattern;

            bool operator()(uint row, uint col) override {
                return ( ((row + col) % 2) + ((row * col) % 3)) % 2 == 0;
            }
        };

    }
};



