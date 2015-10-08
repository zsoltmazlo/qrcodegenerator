/*
 * File:   MainWindow.cpp
 * Author: zsoltmazlo
 *
 * Created on April 20, 2015, 6:50 PM
 */

#include "MainWindow.h"

#include <iostream>
#include "qr/encoding/IEncoder.h"

#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QLineEdit>

#include "qr/utils/QrException.h"
#include "qr/visualization/SvgVisualizer.h"
#include "qr/encoding/EncodingHelper.h"

using namespace qr;

//int main() {
//    qr::QRCodeGenerator gen;
//
//    std::string proba = "C++11 rocks! And Czirkos Zoli also rocks. Definitely. árvíztűrő tükörfúrógép";
//
//    try {
////        gen << encoding::ErrorCorrectionLevel::L;
//        gen << proba;
//        gen << new qr::visualization::SvgVisualizer("qtnelkul");
//        gen.generate();
//    } catch (QRException& qrexp) {
//        std::cout << qrexp.what() << std::endl;
//    }
//
//    return 0;
//}

//int main() {
//
//    // EC tesztek
//
//    std::vector<int> block1 = {67, 85, 70, 134, 87, 38, 85, 194, 119, 50, 6, 18, 6, 103, 38};
//    std::vector<int> block2 = {246, 246, 66, 7, 118, 134, 242, 7, 38, 86, 22, 198, 199, 146, 6};
//    std::vector<int> block3 = {182, 230, 247, 119, 50, 7, 118, 134, 87, 38, 82, 6, 134, 151, 50, 7};
//    std::vector<int> block4 = {70, 247, 118, 86, 194, 6, 151, 50, 16, 236, 17, 236, 17, 236, 17, 236};
//
//    auto vectorprint = [](std::vector<int> v) {
//        for (auto i : v) {
//            std::cout << i << ", ";
//        }
//        std::cout << std::endl;
//    };
//
//    std::vector<int> ec1 = correction::CorrectionHelper::getErrorCorrectionCodewords(block1, 18);
//    std::vector<int> ec2 = correction::CorrectionHelper::getErrorCorrectionCodewords(block2, 18);
//    std::vector<int> ec3 = correction::CorrectionHelper::getErrorCorrectionCodewords(block3, 18);
//    std::vector<int> ec4 = correction::CorrectionHelper::getErrorCorrectionCodewords(block4, 18);
//
//    vectorprint(block1);
//    vectorprint(ec1);
//    vectorprint(block2);
//    vectorprint(ec2);
//    vectorprint(block3);
//    vectorprint(ec3);
//    vectorprint(block4);
//    vectorprint(ec4);
//
//
//
//    return 0;
//}

// TODO save as svg gomb!
// TODO save as bitmap gomb!

int main(int argc, char *argv[]) {
    // A QApplication instance is necessary if fonts are used in the SVG
    QApplication app(argc, argv);
    
    std::shared_ptr<client::MainWindow> window(new client::MainWindow());
    window->show();
    
    return app.exec();
}

client::MainWindow::MainWindow() {
    widget.setupUi(this);

    // setting up error correction types
    widget.version->addItem("Low", (int) qr::encoding::ErrorCorrectionLevel::L);
    widget.version->addItem("Medium", (int) qr::encoding::ErrorCorrectionLevel::M);
    widget.version->addItem("High", (int) qr::encoding::ErrorCorrectionLevel::Q);
    widget.version->addItem("Best", (int) qr::encoding::ErrorCorrectionLevel::H);
    // if user not choosing a version, it should be with a better error correction
    widget.version->setCurrentIndex(2);

    // setting up masking options
    widget.mask->addItem("Use best", 8);
    widget.mask->addItem("Mask #0", 0);
    widget.mask->addItem("Mask #1", 1);
    widget.mask->addItem("Mask #2", 2);
    widget.mask->addItem("Mask #3", 3);
    widget.mask->addItem("Mask #4", 4);
    widget.mask->addItem("Mask #5", 5);
    widget.mask->addItem("Mask #6", 6);
    widget.mask->addItem("Mask #7", 7);
    widget.version->setCurrentIndex(0);

    // when new message writed, automatically generate new code
    connect(widget.message, SIGNAL(textChanged()),
            this, SLOT(messageChangedHandler()));

    // when new error correction level is setted then generate new code
    connect(widget.version, SIGNAL(currentIndexChanged(int)),
            this, SLOT(errorCorrectionLevelChangedHandler(int)));

    // when new mask is setted then generate new code
    connect(widget.mask, SIGNAL(currentIndexChanged(int)),
            this, SLOT(errorCorrectionLevelChangedHandler(int)));

    // creating a generator for later usage
    generator = new QRCodeGenerator(&qConsole);

    // setting up output format
    *generator << new visualization::SvgVisualizer("testfile");
}

client::MainWindow::~MainWindow() {
}

void client::MainWindow::errorCorrectionLevelChangedHandler(int) {
    this->messageChangedHandler();
}

void client::MainWindow::messageChangedHandler() {
    QByteArray arr = widget.message->toPlainText().toUtf8();
    std::string message = QString(arr).toStdString();
    std::replace(message.begin(), message.end(), '\n', ' ');
    this->generateCode(message);
}

void client::MainWindow::generateCode(std::string& message) {
    // casting version to enumeration and setting up generator with it
    *generator << (encoding::ErrorCorrectionLevel)widget.version->currentIndex();

    // getting mask information
    *generator << (masking::Mask)widget.mask->itemData(widget.mask->currentIndex()).toInt();

    // clear console before showing it on output
    qConsole.str("");
    qConsole.clear();

    // get standard message from input and setting up generator with it
    *generator << message;
    generator->generate();

    widget.console->setText(QString::fromStdString(qConsole.str()));

    // loading file into UI
    QSvgRenderer renderer{QString::fromStdString(generator->getFileName())};
    QImage image(500, 500, QImage::Format_ARGB32);
    QPainter painter{&image};
    renderer.render(&painter);
    widget.image->setPixmap(QPixmap::fromImage(image));
    widget.image->show();
    this->repaint();
}


