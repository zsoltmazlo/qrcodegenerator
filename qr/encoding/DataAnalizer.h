/* 
 * File:   DataAnalizer.h
 * Author: zsoltmazlo
 *
 * Created on April 27, 2015, 6:38 PM
 */

#pragma once

#include "IEncoder.h"
#include "qr/utils/ByteArray.h"

#include <memory>

namespace qr {

    namespace encoding {

        class DataAnalizer {
            // enumeration for storing only encoder type
            // instead of storing an actual instance of them
            enum EncoderType {
                NotSpecified,
                Numberic,
                Alphanumberic,
                Byte
            };

            EncoderType type;

        public:

            DataAnalizer();

            /**
             * analizing message for decide which encoder would be proper
             * to encode message
             * 
             * @param message
             */
            void analize(utils::ByteArray& message);

            std::shared_ptr<IEncoder> getEncoder();

            virtual ~DataAnalizer();
        };
    }


}

