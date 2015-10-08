/* 
 * File:   Masker.h
 * Author: zsoltmazlo
 *
 * Created on May 3, 2015, 8:16 PM
 */

#pragma once

#include "qr/utils/ModuleMatrix.h"
#include "IMaskPattern.h"

#include <functional>

namespace qr {
    namespace masking {

        class Masker : public IMaskPattern {
            std::function<bool(uint, uint) > maskerFunction;
        public:

            Masker(utils::ModuleMatrix matrix, std::function<bool(uint, uint) > fn) : IMaskPattern(matrix), maskerFunction(fn) { }

            virtual bool operator()(uint row, uint col) override;
        };

    }
}



