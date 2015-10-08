/* 
 * File:   IVisualizer.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 11:11 AM
 */

#pragma once

#include "qr/utils/ModuleMatrix.h"
#include "qr/encoding/IEncoder.h"

namespace qr {

    namespace visualization {

        /**
         * Interface responsible for visualize the generated QR code
         * 
         * I separated the encoding and the visualizing responsiblities to able
         * to replace visualization if necessary
         */
        class IVisualizer {
        public:

            virtual void setQRVersion(unsigned int version) = 0;

            virtual void setModuleDimension(unsigned int dimension) = 0;

            virtual void setFileName(std::string& fileName) = 0;

            virtual std::string getFileName() = 0;

            virtual void visualize(const utils::ModuleMatrix& matrix) = 0;

            virtual ~IVisualizer() { }
        };


    }
}

