/* 
 * File:   SvgVisualizer.h
 * Author: zsoltmazlo
 *
 * Created on April 25, 2015, 11:20 AM
 */

#pragma once
#include "IVisualizer.h"
#include "qr/utils/simple_svg_1.0.0.hpp"
#include "qr/utils/ModuleMatrix.h"

namespace qr {

    namespace visualization {

        class SvgVisualizer : public IVisualizer {
            unsigned int mModuleDim;
            unsigned int mVersion;
            svg::Document* mDocument = nullptr;
            svg::Dimensions mDimension;
            svg::Layout mLayout;
            std::string mFileName;

        public:
            
            SvgVisualizer(std::string fileName) {
                setFileName(fileName);
            }

            void setQRVersion(unsigned int version) override;

            void setFileName(std::string& fileName) override;
            
            std::string getFileName() override;

            void setModuleDimension(unsigned int dimension) override;
            
            void visualize(const utils::ModuleMatrix& matrix) override;

            ~SvgVisualizer();
        };
    }
}





