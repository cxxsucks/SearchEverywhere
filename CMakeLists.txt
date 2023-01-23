cmake_minimum_required(VERSION 3.5)
enable_language(CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
add_subdirectory(previewer)
add_subdirectory(assets)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(SEEV_SYSTEM_ORIENT "Use System orient Library" OFF)
if (NOT SEEV_SYSTEM_ORIENT)
    include(FetchContent)
    FetchContent_Declare(orient
        GIT_REPOSITORY https://github.com/cxxsucks/orient
        GIT_TAG master)
    FetchContent_MakeAvailable(orient)
else(NOT SEEV_SYSTEM_ORIENT)
    find_package(orie REQUIRED)
endif(NOT SEEV_SYSTEM_ORIENT)

project(seev VERSION 0.3.1)
set(seev_src
    main.cc
    widgets/homepagewidget.cpp widgets/homepagewidget.hpp widgets/homepagewidget.ui
    widgets/oriepredselector.hpp widgets/oriepredselector.cpp widgets/oriepredselector.ui
    widgets/orieconfwidget.cpp widgets/orieconfwidget.hpp widgets/orieconfwidget.ui
    widgets/locatejobwidget.cpp widgets/locatejobwidget.hpp widgets/locatejobwidget.ui
    widgets/startpathconfwidget.ui
    util/fileinfomodel.cpp util/fileinfomodel.hpp
    assets/seev_rsc.qrc
    assets/win_ico.rc
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE stolenLogo.icns)
    set(app_icon_macos assets/search_img/stolenLogo.icns)
    set_source_files_properties(${app_icon_macos} PROPERTIES
                                MACOSX_PACKAGE_LOCATION "Resources")
endif(APPLE)

if(${QT_VERSION_MAJOR} EQUAL 6)
    if(APPLE)
        qt_add_executable(seev MANUAL_FINALIZATION 
                        ${seev_src} ${app_icon_macos})
    elseif(WIN32)
        qt_add_executable(seev WIN32 MANUAL_FINALIZATION ${seev_src})
    else()
        qt_add_executable(seev MANUAL_FINALIZATION ${seev_src})
    endif()
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET seev APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    qt6_add_translations(seev TS_FILES seev_es.ts seev_fr.ts seev_zh_CN.ts
                         QM_FILES_OUTPUT_VARIABLE seev_qm_files)

else(${QT_VERSION_MAJOR} EQUAL 6)
    if(APPLE)
        add_executable(seev ${seev_src} ${app_icon_macos})
    elseif(WIN32)
        add_executable(seev WIN32 ${seev_src})
    elseif(ANDROID)
        add_library(seev ${seev_src})
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(seev ${seev_src})
    endif()
    qt5_add_translation(seev TS_FILES seev_es.ts seev_fr.ts seev_zh_CN.ts
                        QM_FILES_OUTPUT_VARIABLE seev_qm_files)
endif(${QT_VERSION_MAJOR} EQUAL 6)

set_target_properties(seev PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(seev PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets orie::orie previewer)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(seev)
endif(${QT_VERSION_MAJOR} EQUAL 6)
install(TARGETS seev DESTINATION bin)
install(FILES ${seev_qm_files} DESTINATION share/seev)
