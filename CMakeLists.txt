## 
cmake_minimum_required(VERSION 3.2)

project(TsReader)

#
# Directory with modules
#
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

#
# Configuration variables
#
option(INSTALL_HEADERS "Install the headers" OFF)
option(INSTALL_CLIENT "Install the Client" OFF)

#
# Extra compiler / linker options
#
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wformat=2")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#
# Packages to use
#
#find_package(Threads REQUIRED)

find_package(wxWidgets REQUIRED core base)

#
# Include directories
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(wxWidgets_FOUND)
  include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

#
# Sources to compile
#
set(REPO_SOURCES
    src/TsDbg.cpp
    src/TsFile.cpp
    src/TsLogFrame.cpp
    src/TsMsg.cpp
    src/TsReaderApp.cpp
    src/TsReaderFrame.cpp
    src/TsThread.cpp
)

#
# Targets
#
add_executable(TsReader ${REPO_SOURCES})

set_property(TARGET TsReader PROPERTY CXX_STANDARD 17)

#target_link_libraries(${PROGRAM_NAME} Threads::Threads)

if(wxWidgets_FOUND)
  target_link_libraries(TsReader ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)
