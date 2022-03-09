cmake_minimum_required(VERSION 3.20)
project(webtrakr VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -Os")

#set(ANDROID True)
#set(QT_ANDROID_NDK_ROOT "C:\\Android\\android-ndk-r23")
set(ANDROID_STL "c++_static")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(ANDROID)
    set(CMAKE_TOOLCHAIN_FILE "C:/Android/ndk/21.3.6528147/build/cmake/android.toolchain.cmake")
    add_library(webtrakr SHARED main.cpp mainwidget.cpp web-helper.cpp qrightclickbutton.cpp resources.qrc)

else()
    add_executable(webtrakr main.cpp mainwidget.cpp web-helper.cpp qrightclickbutton.cpp resources.qrc)
endif()

target_link_libraries(webtrakr Qt5::Core Qt5::Widgets curl pthread)

#SET(CMAKE_BUILD_TYPE Debug)

#set_target_properties(webtrakr PROPERTIES
#        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#        )

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(untitled)
#endif()