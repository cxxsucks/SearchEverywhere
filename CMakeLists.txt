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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
             Widgets LinguistTools Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
             Widgets LinguistTools Multimedia MultimediaWidgets)

set(TS_FILES seev_zh_CN.ts)

set(PROJECT_SOURCES
    main.cpp
    widgets/navwidget.cpp widgets/navwidget.hpp widgets/navwidget.ui
    widgets/oriepredselector.hpp widgets/oriepredselector.cpp widgets/oriepredselector.ui
    widgets/orieconfwidget.cpp widgets/orieconfwidget.hpp widgets/orieconfwidget.ui
    widgets/locatejobwidget.cpp widgets/locatejobwidget.hpp widgets/locatejobwidget.ui
    # util/previewer.cpp util/previewer.hpp
    # util/fil
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(seev
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET seev APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(seev SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(seev
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(seev PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Multimedia
                      Qt${QT_VERSION_MAJOR}::MultimediaWidgets
                      orie
)

set_target_properties(seev PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(seev)
endif()
