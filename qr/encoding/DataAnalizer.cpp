
#include "DataAnalizer.h"
#include "NumberEncoder.h"
#include "AlphanumbericEncoder.h"
#include "ByteEncoder.h"
#include "EncodingHelper.h"
#include "qr/QRCodeGenerator.h"

#include <iostream>

using namespace qr::encoding;

DataAnalizer::DataAnalizer() : type(NotSpecified) {

}

void DataAnalizer::analize(utils::ByteArray& message) {
    /**
     * method: 
     * 
     * Numberic only could hold 0-9, so if there is another character -> NOT Numberic
     * ANumeric only could hold A-Z0-9 and some puctual characters, if there any 
     * other -> NOT Anumeric
     * 
     * if none of the previous two are usable -> Byte
     */

    // it would be sooo sexy :( but g++ 4.8 not supporting std::regex
    //        std::smatch matcher;
    //        std::regex somethingNotNumber("[^0-9]+");
    //        std::regex somethingNotANum("[^0-9A-Z $%*+-.\/:]+", std::regex_constants::basic);
    //        if (!std::regex_match(message, matcher, somethingNotNumber)) {
    //            type = Numberic;
    //            return;
    //        }
    //        if (!std::regex_match(message, matcher, somethingNotANum)) {
    //            type = Alphanumberic;
    //            return;
    //        }

    bool onlyNumber = true, onlyAnum = true;
    for (uint i = 0; i < message.length(); ++i) {
        char current = (char) message[i];
        // if its not a number, then it could not be a NumberEncoder
        if (!EncodingHelper::isNumber(current)) {
            onlyNumber = false;
        }
        // if its not an alpanumberic character, then it could not be a AlphanumericEncoder
        if (!EncodingHelper::isAlphabetic(current, true)) {
            if (!EncodingHelper::isPunction(current)) {
                onlyAnum = false;
            }
        }
        // if not alphanumeric or not a number, there is no need for further iteration
        if (!onlyAnum && !onlyNumber) {
            break;
        }
    }

    // true = 1, false = 0
    utils::ByteArray dataWithBOM;
    uint result = (((uint) onlyAnum) << 1) | ((uint) onlyNumber);
    switch (result) {
            // false, false -> BYTE
        case 0:
            type = Byte;

            // setting BOM!
            dataWithBOM.reserve(message.length() + 3);
            dataWithBOM[0] = 0xEF;
            dataWithBOM[1] = 0xBB;
            dataWithBOM[2] = 0xBF;
            for (uint i = 0; i < message.length(); i++) {
                dataWithBOM[i + 3] = message[i];
            }
            message = dataWithBOM;
            break;
            // false, true -> NUMBER
        case 1:
            // true, true --> NUMERIC! (not ALPHANUMERIC)
        case 3:
            type = Numberic;
            
            // there is some bug if the first character(s) are zero, 
            // so fallback on Alphanumeric if the first is zero or if its less
            // then 5 characters
            if( message.length()<5 || message[0] == '0' ) {
                type = Alphanumberic;
            }
            break;
            // true, false -> ALPHANUMERIC
        case 2:
            type = Alphanumberic;
            break;
    }

}

std::shared_ptr<IEncoder> DataAnalizer::getEncoder() {
    switch (type) {
        case Numberic:
            //            std::cout << "type: Numberic" << std::endl;
            return std::make_shared<NumberEncoder>();

        case Alphanumberic:
            //            std::cout << "type: Alphanumberic" << std::endl;
            return std::make_shared<AlphanumbericEncoder>();

        case Byte:
        default:
            //            std::cout << "type: Byte" << std::endl;
            return std::make_shared<ByteEncoder>();
    }
}

DataAnalizer::~DataAnalizer() {

}



