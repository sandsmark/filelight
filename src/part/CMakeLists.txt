
include_directories(${CMAKE_SOURCE_DIR}/filelight/src ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(radialMap)


########### next target ###############

set(filelight_PART_SRCS
    part.cpp
    scan.cpp
    progressBox.cpp
    Config.cpp
    settingsDialog.cpp
    fileTree.cpp
    localLister.cpp
    remoteLister.cpp
    summaryWidget.cpp)

kde4_add_ui_files(filelight_PART_SRCS dialog.ui)

kde4_add_plugin(filelight WITH_PREFIX ${filelight_PART_SRCS})

target_link_libraries(filelight ${KDE4_KDECORE_LIBS} radialmap ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS filelight DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS     = radialMap
#INCLUDES    = $(all_includes) -I$(top_srcdir)/filelight/src
#METASOURCES = AUTO
#
##Part
#kde_module_LTLIBRARIES  = libfilelight.la
#libfilelight_la_LIBADD  = ./radialMap/libradialmap.la $(LIB_KFILE) $(LIB_KPARTS) $(LIB_KDEUI) $(LIB_QT)
#libfilelight_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
#libfilelight_la_SOURCES = \
#   dialog.ui \
#   part.cpp \
#   scan.cpp \
#   progressBox.cpp \
#   Config.cpp \
#   settingsDialog.cpp \
#   fileTree.cpp \
#   localLister.cpp \
#   remoteLister.cpp \
#   summaryWidget.cpp
