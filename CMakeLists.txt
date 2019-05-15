cmake_minimum_required(VERSION 3.5.1)
project(RGBDVideoRecorder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)

find_package(Threads REQUIRED)
find_package(realsense2 REQUIRED)

include_directories(${realsense2_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories("./recorder")
include_directories("./interface")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets CONFIG REQUIRED)


file(GLOB_RECURSE RGBD_HEADERS "recorder/*.h")
file(GLOB_RECURSE RGBD_SOURCES "recorder/*.cpp")
FILE(GLOB_RECURSE QtHeaders  "interface/*.h")
FILE(GLOB_RECURSE QtSources  "interface/*.cpp")
FILE(GLOB_RECURSE QtForms  "interface/*.ui")
FILE(GLOB_RECURSE QtRessources  "interface/*.qrc")


list(APPEND RGBD_HEADERS ${QtHeaders} ${QtRessources})
list(APPEND RGBD_SOURCES ${QtSources})

FILE(GLOB_RECURSE QTFen RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "interface/*.ui")

QT5_WRAP_CPP(header_mocs ${QtHeaders})
QT5_WRAP_UI(form_headers ${QtForms})
qt5_add_resources(RCC_SOURCES ${QtRessources})
add_executable(${CMAKE_PROJECT_NAME} main.cpp  ${RGBD_HEADERS} ${RGBD_SOURCES} ${header_mocs} ${form_headers} ${RCC_SOURCES} ${QM_FILES})

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${realsense2_LIBRARY})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} Qt5::Widgets stdc++fs)


