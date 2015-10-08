cmake_minimum_required(VERSION 3.0.2)

project(qr_code_generator)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 
#set(CMAKE_AUTORCC ON) # resource compiler

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5Svg)

include_directories(
        ${CMAKE_SOURCE_DIR}/qr)

SET(FORMS
        MainWindow.ui)

SET(SOURCES
        MainWindow.cpp
	qr/correction/ErrorCorrection.cpp
	qr/encoding/AlphanumbericEncoder.cpp
	qr/encoding/ByteEncoder.cpp
	qr/encoding/DataAnalizer.cpp
	qr/encoding/EncodingHelper.cpp
	qr/encoding/NumberEncoder.cpp
	qr/masking/IMaskPattern.cpp
	qr/masking/Masker.cpp
	qr/QRCodeGenerator.cpp
	qr/utils/ByteArray.cpp
	qr/utils/Configuration.cpp
	qr/utils/LogFormatter.cpp
	qr/utils/ModuleMatrix.cpp
	qr/visualization/SvgVisualizer.cpp)

add_executable(qr_code_generator ${FORMS} ${SOURCES})
target_link_libraries(
	qr_code_generator
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::Svg)

install(TARGETS qr_code_generator DESTINATION .)

set(CPACK_PACKAGE_NAME "QrCode Generator")
set(CPACK_PACKAGE_VENDOR "Mázló Zsolt")
set(CPACK_PACKAGE_VERSION "0.9.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QrCode Generator")

include(CPack)
