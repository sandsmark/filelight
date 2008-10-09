
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

install(FILES  filelight_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  filelight.desktop DESTINATION ${XDG_APPS_INSTALL_DIR}/kde)
install(FILES  filelightrc DESTINATION /share/config)
install(FILES  filelight_partui.rc filelightui.rc DESTINATION ${DATA_INSTALL_DIR}/filelight)

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
