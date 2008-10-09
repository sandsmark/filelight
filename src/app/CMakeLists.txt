
include_directories(${CMAKE_SOURCE_DIR}/filelight/src ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(filelight_SRCS historyAction.cpp mainWindow.cpp main.cpp)

kde4_add_executable(filelight ${filelight_SRCS})

target_link_libraries(filelight ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS filelight ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes) -I$(top_srcdir)/filelight/src
#METASOURCES = AUTO
#
## Application
#bin_PROGRAMS = filelight
#filelight_SOURCES = historyAction.cpp mainWindow.cpp main.cpp
#filelight_LDADD = $(LIB_KPARTS) $(LIB_KDEUI)
#filelight_LDFLAGS = $(all_libraries) $(KDE_RPATH)
