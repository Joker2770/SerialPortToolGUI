# SerialPortToolGUI
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
./SerialPortToolGUI
~~~

## Build on windows
~~~
git clone https://github.com/Joker2770/SerialPortToolGUI.git
cd SerialPortToolGUI
git submodule update --init --recursive
~~~
build with cmake-gui.
