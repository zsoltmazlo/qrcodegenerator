/* 
 * File:   MainWindow.h
 * Author: zsoltmazlo
 *
 * Created on April 20, 2015, 6:50 PM
 */

#pragma once

#include "ui_MainWindow.h"
#include <QObject>
#include <sstream>

#include "qr/QRCodeGenerator.h"
#include "qr/utils/ByteArray.h"

namespace client {

    class MainWindow : public QWidget {
        Q_OBJECT
    public:
        MainWindow();
        virtual ~MainWindow();

    public slots:
        
        void messageChangedHandler();
        
        void errorCorrectionLevelChangedHandler(int index);

    private:
        Ui::MainWindow widget;
        qr::QRCodeGenerator* generator;
        
        // creating an ostream object for storing debug data
        std::ostringstream qConsole;

        void generateCode(std::string& message);
    };

};

