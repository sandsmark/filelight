
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

install(FILES  filelight_part.desktop DESTINATION /share/services)
install(FILES  filelight.desktop DESTINATION /share/applications/kde)
install(FILES  filelightrc DESTINATION /share/config)
install(FILES  filelight_partui.rc filelightui.rc DESTINATION /share/apps/filelight)

kde4_install_icons(${ICON_INSTALL_DIR})



#original Makefile.am contents follow:

#KDE_ICON = AUTO
#
#rcdir = $(kde_datadir)/filelight
#rc_DATA = filelight_partui.rc filelightui.rc
#
#configdir = $(kde_confdir)
#config_DATA = filelightrc
#
#xdg_apps_DATA = filelight.desktop
#
#partdesktopdir   = $(kde_servicesdir)
#partdesktop_DATA = filelight_part.desktop
