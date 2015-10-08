/* 
 * File:   IMaskPattern.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 5:48 PM
 */

#pragma once

#include "qr/utils/ModuleMatrix.h"

#include <functional>

namespace qr {

    // generally we are using uint for every place in qr namespace,
    // so it could be better if defining it in qr namespace
    //    using uint = unsigned int;
    // netbeans coloring the type in this form
    typedef unsigned int uint;

    namespace masking {

        class IMaskPattern {
        public:

            IMaskPattern(utils::ModuleMatrix matrix);

            virtual bool operator()(uint row, uint col);

            void apply(uint maskNumber);

            uint evaulate();

            utils::ModuleMatrix& getMatrix();

            virtual ~IMaskPattern() { }

        protected:

            utils::ModuleMatrix mMatrix;

            uint evaulateCondition1();

            uint evaulateCondition2();

            uint evaulateCondition3();

            uint evaulateCondition4();

        };

       

    }
}



