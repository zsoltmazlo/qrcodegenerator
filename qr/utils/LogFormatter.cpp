#include "LogFormatter.h"

using namespace qr::utils;

LogFormatter::LogFormatter() : columnCount(0), mConsole(nullptr), mFill(' '), mWidth(0) {
}

void LogFormatter::operator()() {
    if (mConsole == nullptr) {
        return;
    }
    if (mWidth > 0) {
        mConsole->fill(mFill);
        mConsole->width(mWidth);
    }
    if (columnCount == maxColumn) {
        *mConsole << std::endl;
    }
    columnCount = (columnCount+1) % maxColumn;
}

LogFormatter& LogFormatter::reset() {
    columnCount = 0;
    mFill = ' ';
    mWidth = 0;
    return *this;
}

void LogFormatter::setConsole(std::ostream* console) {
    mConsole = console;
}

LogFormatter& LogFormatter::setWidth(uint width, char fill) {
    mWidth = width;
    mFill = fill;
    return *this;
}





