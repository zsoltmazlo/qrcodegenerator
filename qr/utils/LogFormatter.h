/* 
 * File:   LogFormatter.h
 * Author: zsoltmazlo
 *
 * Created on May 3, 2015, 8:51 PM
 */

#pragma once

#include <iostream>

namespace qr {

    namespace utils {

        class LogFormatter {
            static constexpr uint maxColumn = 6;
            uint columnCount;
            std::ostream* mConsole;
            char mFill;
            uint mWidth;

        public:

            LogFormatter();

            LogFormatter& setWidth(uint width, char fill = ' ');

            void setConsole(std::ostream* console);

            LogFormatter& reset();

            void operator()();
        };
    }
}



