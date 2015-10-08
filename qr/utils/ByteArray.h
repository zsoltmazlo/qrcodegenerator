/* 
 * File:   ByteArray.h
 * Author: zsoltmazlo
 *
 * Created on May 2, 2015, 11:35 AM
 */

#pragma once

#include <cstdlib>

namespace qr {

    typedef unsigned int uint;

    namespace utils {

        class ByteArray {
            uint* mData;
            uint mLen;

        public:

            ByteArray();

            void reserve(uint n);

            uint length() const;

            uint operator[](const uint i) const;

            uint& operator[](const uint i);

            ByteArray& operator=(const ByteArray& orig);

            ~ByteArray();
        };

    }
}



