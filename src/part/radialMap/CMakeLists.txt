include_directories(${CMAKE_SOURCE_DIR}/filelight/src/part ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

set(radialmap_STAT_SRCS
    widget.cpp
    builder.cpp
    map.cpp
    widgetEvents.cpp
    labels.cpp
    segmentTip.cpp)

kde4_add_library(radialmap STATIC ${radialmap_STAT_SRCS} )
find_package(Blitz)
