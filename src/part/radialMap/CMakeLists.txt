
include_directories(${CMAKE_SOURCE_DIR}/filelight/src/part ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(radialmap_STAT_SRCS
    widget.cpp
    builder.cpp
    map.cpp
    widgetEvents.cpp
    labels.cpp
    segmentTip.cpp)

kde4_add_library(radialmap STATIC ${radialmap_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/filelight/src/part $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libradialmap.la
#libradialmap_la_SOURCES = widget.cpp builder.cpp map.cpp widgetEvents.cpp labels.cpp segmentTip.cpp
