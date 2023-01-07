# SerialPortToolGUI

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3efc027964764539b02e58ae1b8ba422)](https://app.codacy.com/gh/Joker2770/SerialPortToolGUI?utm_source=github.com&utm_medium=referral&utm_content=Joker2770/SerialPortToolGUI&utm_campaign=Badge_Grade_Settings)
[![serialporttoolgui](https://snapcraft.io/serialporttoolgui/badge.svg)](https://snapcraft.io/serialporttoolgui)

A Cross-platform serial debug tool.

## Dependencies
Required:
* [cmake](http://www.cmake.org) - buildsystem
* g++ (>= 7.4.0 recommended)
* gcc (>= 7.4.0 recommended)
* gtk+3.0 (>= 3.22)

## Build on Linux
~~~
git clone https://github.com/Joker2770/SerialPortToolGUI.git
cd SerialPortToolGUI
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
~~~

## Build on windows
~~~
git clone https://github.com/Joker2770/SerialPortToolGUI.git
cd SerialPortToolGUI
git submodule update --init --recursive
~~~
build with cmake-gui.
