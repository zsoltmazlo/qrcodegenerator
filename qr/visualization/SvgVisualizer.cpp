#include "SvgVisualizer.h"

using namespace qr::visualization;
using namespace qr::utils;

void SvgVisualizer::setFileName(std::string& fileName) {
    mFileName = fileName + ".svg";
}

std::string SvgVisualizer::getFileName() {
    return mFileName;
}

void SvgVisualizer::setModuleDimension(unsigned int dimension) {
    mModuleDim = dimension;
}

void SvgVisualizer::setQRVersion(unsigned int version) {
    // calculate dimensions of image
    mVersion = version;
    unsigned int dim = ((version - 1)*4 + 21);
    mDimension = svg::Dimensions(dim, dim);
    svg::Dimensions documentDimensions = svg::Dimensions((dim + 8) * mModuleDim, (dim + 8) * mModuleDim);

    // now we now the layout also
    mLayout = svg::Layout(documentDimensions, svg::Layout::TopLeft, mModuleDim);
    mDocument = new svg::Document(mFileName, mLayout);
}

void SvgVisualizer::visualize(const ModuleMatrix& matrix) {

    svg::Rectangle bg = svg::Rectangle(svg::Point(0, 0), mDimension.width + 8, mDimension.height + 8, svg::Fill(svg::Color::White));
    *mDocument << bg;

    svg::Fill moduleFill;
    Rectangle d = matrix.getDimension();
    for (unsigned int row = 0; row < d.height; ++row) {
        for (unsigned int col = 0; col < d.width; ++col) {

            switch (matrix(row, col)) {
                case 0:
                    moduleFill = svg::Fill(svg::Color::White);
                    break;

                case 1:
                    moduleFill = svg::Fill(svg::Color::Black);
                    break;

                case 2:
                    moduleFill = svg::Fill(svg::Color::Blue);
                    break;
            }

            svg::Rectangle module = svg::Rectangle(svg::Point(col + 4, row + 4), 1, 1, moduleFill);
            *mDocument << module;
        }
    }

    mDocument->save();
}

SvgVisualizer::~SvgVisualizer() {
    if (mDocument != nullptr) {
        delete mDocument;
    }
}
