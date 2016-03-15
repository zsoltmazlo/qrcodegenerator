# qrcodegenerator
An QR code generator application written in c++11. I made it for homework of a university subject called "C11 and C++11 Programming" and it was a individual task choosen by myself.

## install
The generator module placed in the qr folder could be used without any external library, the only used library called simple_svg_1.0.0 is directly attached (as source) to the project.

There is also a UI module developed in Qt5, where I need to use some extra qt packages, called Svg, it should be installed before compiling this source.

On Ubuntu 14.04, you can install Qt from with a command as follows:
`sudo apt-get install qt5-default qttools5-dbg qttools5-dev qttools5-doc qttools5-dev-tools libqt5svg5 libqt5svg5-dev build-essential`

Also, I'm created a cmakelist file for compiling code using cmake. On Unix systems, the compiling needs only 2 command:

`cmake .`

`make`

