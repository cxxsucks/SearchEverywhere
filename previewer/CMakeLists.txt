set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PREVIEWER_SRC 
    previewer.cpp previewer.hpp previewer.ui
    imagePreviewer/imagepreviewer.cpp
    imagePreviewer/imagepreviewer.h
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

option(SEEV_PREVIEW_MEDIA "Preview Multimedia files in search result" OFF)

if(SEEV_PREVIEW_MEDIA)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
                 Multimedia MultimediaWidgets REQUIRED)
    add_compile_definitions(_SEEV_PREVIEW_MEDIA)
    set(s mediaPreviewer/${QT_VERSION_MAJOR})

    # videowidget.cpp is the same across 5 and 6
    set(PREVIEWER_SRC ${PREVIEWER_SRC}
        ${s}/mediapreviewer.cpp ${s}/playercontrols.cpp
        ${s}/../videowidget.cpp ${s}/mediapreviewer.h
        ${s}/playercontrols.h ${s}/../videowidget.h
    )
    unset(s)
endif(SEEV_PREVIEW_MEDIA)

add_library(previewer ${PREVIEWER_SRC})

if(SEEV_PREVIEW_MEDIA)
    target_link_libraries(previewer PRIVATE 
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    )
endif(SEEV_PREVIEW_MEDIA)
target_link_libraries(previewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
