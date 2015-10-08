/* 
 * File:   QrException.h
 * Author: zsoltmazlo
 *
 * Created on May 3, 2015, 10:20 PM
 */

#pragma once


#include <exception>
#include <string>

namespace qr {

    class QRException : public std::exception {
        std::string message;
    public:

        QRException(const std::string& msg) : message(msg) { }

        virtual const char* what() const throw () {
            return message.c_str();
        }
    };
}



