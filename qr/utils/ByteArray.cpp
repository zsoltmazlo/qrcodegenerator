

#include <stdexcept>

#include "ByteArray.h"
#include "QrException.h"

using namespace qr::utils;

ByteArray::ByteArray() {
    mData = nullptr;
    mLen = 0;
}

void ByteArray::reserve(uint n) {
    if (mData != nullptr) {
        delete mData;
    }
    mLen = n;
    mData = (uint*) malloc(mLen * sizeof (uint));
    for (uint i = 0; i < mLen; ++i) {
        mData[i] = 0;
    }
}

uint ByteArray::length() const {
    return mLen;
}

ByteArray& ByteArray::operator=(const ByteArray& orig) {

    if (this != &orig) {
        reserve(orig.length());
        for (uint i = 0; i < mLen; ++i) {
            mData[i] = orig.mData[i];
        }
    }
    return *this;
}

uint ByteArray::operator[](const uint i) const {
    if (i < mLen) {
        return mData[i];
    }
    throw QRException("Index is out of range of ByteArray!");
}

uint& ByteArray::operator[](const uint i) {
    if (i < mLen) {
        return mData[i];
    }
    throw QRException("Index is out of range of ByteArray!");
}

ByteArray::~ByteArray() {
    if (mData != nullptr) {
        delete mData;
    }
}
