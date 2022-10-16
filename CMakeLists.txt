cmake_minimum_required(VERSION 3.5)

project(seev VERSION 0.1 LANGUAGES CXX)

add_subdirectory(orient)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/orient/include)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
             Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS
             Widgets LinguistTools)
add_subdirectory(previewer)

set(PROJECT_SOURCES
    main.cpp
    widgets/homepagewidget.cpp widgets/homepagewidget.hpp widgets/homepagewidget.ui
    widgets/oriepredselector.hpp widgets/oriepredselector.cpp widgets/oriepredselector.ui
    widgets/orieconfwidget.cpp widgets/orieconfwidget.hpp widgets/orieconfwidget.ui
    widgets/locatejobwidget.cpp widgets/locatejobwidget.hpp widgets/locatejobwidget.ui
    widgets/startpathconfwidget.ui
    util/fileinfomodel.cpp util/fileinfomodel.hpp
    assets/seev_rsc.qrc
)

qt_add_executable(seev MANUAL_FINALIZATION ${PROJECT_SOURCES})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET seev APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
qt_add_translations(seev TS_FILES seev_zh_CN.ts)
target_link_libraries(
    seev PRIVATE Qt6::Widgets orie pcre2-8 previewer
)

set_target_properties(seev PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(seev)
