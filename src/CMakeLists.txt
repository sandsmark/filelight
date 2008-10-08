
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(app)
add_subdirectory(part)


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS     = app part
#INCLUDES    = $(all_includes)
#METASOURCES = AUTO
