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

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)

option(ORIE_PREVIEW_MEDIA "Preview Multimedia files in search result" OFF)

if(NEED_PREVIEW_MEDIA)
    find_package(Qt6 COMPONENTS Multimedia MultimediaWidgets REQUIRED)
    add_compile_definitions(_SEEV_PREVIEW_MEDIA)
    set(PREVIEWER_SRC
        mediaPreviewer/mediapreviewer.cpp
        mediaPreviewer/playercontrols.cpp
        mediaPreviewer/videowidget.cpp
        mediaPreviewer/mediapreviewer.h
        mediaPreviewer/playercontrols.h
        mediaPreviewer/videowidget.h
        ${PREVIEWER_SRC}
    )
endif()

add_library(previewer
    ${PREVIEWER_SRC}
)

if(NEED_PREVIEW_MEDIA)
    target_link_libraries(previewer PRIVATE Qt6::Multimedia Qt6::MultimediaWidgets)
endif()
target_link_libraries(previewer PRIVATE Qt6::Widgets)
