cmake_minimum_required(VERSION 3.11)

project(harlequin)

find_package(Qt5 COMPONENTS Quick Qml Gui QuickControls2 CONFIG REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)
if(cmake_build_type_lower MATCHES "debug")
    add_definitions(-DQT_QML_DEBUG)
endif()

qt5_add_resources(RESOURCES resources.qrc)

include_directories(thirdparty)
set(SOURCES c++/main.cpp c++/resultImage.cpp c++/colourWheel.cpp c++/labLut.cpp c++/rgbLut.cpp
  c++/histogram.cpp c++/imageProjection.cpp c++/extendedImage.cpp c++/colourSlice.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Qml Qt5::Quick Qt5::QuickControls2)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
