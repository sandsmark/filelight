
find_package(KDE4 REQUIRED Qt3Support)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS -w -fPIC)

include(KDE4Defaults)

include(MacroLibrary)

#include(ConvenienceLibs.cmake)
#
#include(ManualStuff.cmake)
#
#include(ConfigureChecks.cmake)

include_directories(src/ ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(src)
add_subdirectory(misc)


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS = src misc
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui"` >> rc.cpp
#	LIST=`find src -name \*.h -o -name \*.cpp` ;\
#	if test -n "$$LIST"; then \
#	  $(XGETTEXT) rc.cpp $$LIST -o $(podir)/filelight.pot; \
#	fi
#_SOURCES = release-howto.text
