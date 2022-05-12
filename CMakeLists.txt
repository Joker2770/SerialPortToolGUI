#cmake_minimum_required(VERSION 2.8.3)

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "My multi config types" FORCE)
elseif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_C_FLAGS, "-Wall")
set(CMAKE_CXX_FLAGS, "-Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(SerialPortToolGUI)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)

if(APPLE)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(FOUNDATION_LIBRARY Foundation)
endif()

## Sources
set(serial_SRCS
	serial/src/serial.cc
	serial/include/serial/serial.h
	serial/include/serial/v8stdint.h
)
if(APPLE)
	# If OSX
	list(APPEND serial_SRCS serial/src/impl/unix.cc)
	list(APPEND serial_SRCS serial/src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
	# If unix
	list(APPEND serial_SRCS serial/src/impl/unix.cc)
	list(APPEND serial_SRCS serial/src/impl/list_ports/list_ports_linux.cc)
else()
	# If windows
	list(APPEND serial_SRCS serial/src/impl/win.cc)
	list(APPEND serial_SRCS serial/src/impl/list_ports/list_ports_win.cc)
endif()

## Add serial library
add_library(serial ${serial_SRCS})
if(APPLE)
	target_link_libraries(serial ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
	target_link_libraries(serial rt pthread)
else()
	target_link_libraries(serial setupapi)
endif()

## Include headers
include_directories(serial/include)
include_directories (${GTK3_INCLUDE_DIRS})

## SerialPortTool 
file(GLOB SerialPortTool_SRCS "./sptsrc/*.cpp" "./sptsrc/*.h")
add_executable(${PROJECT_NAME} ${SerialPortTool_SRCS})
add_dependencies(${PROJECT_NAME} serial)
target_link_libraries(${PROJECT_NAME} serial)

add_definitions (${GTK3_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})

# Install
if(UNIX)
	set(CMAKE_INSTALL_PREFIX /usr/local)
	install(TARGETS ${PROJECT_NAME} DESTINATION bin)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SerialPortToolGUI.ui DESTINATION bin)
elseif(WIN32)
	set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/")
	install(TARGETS ${PROJECT_NAME} DESTINATION SerialPortTool)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SerialPortToolGUI.ui DESTINATION SerialPortTool)
endif()

