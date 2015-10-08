

#include "Masker.h"

using namespace qr::masking;


bool Masker::operator()(uint row, uint col) {
    return maskerFunction(row, col);
}




