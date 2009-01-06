include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### install files ###############

install(FILES  filelight_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  filelight.desktop DESTINATION ${XDG_APPS_INSTALL_DIR}/kde)
install(FILES  filelightrc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/config)
install(FILES  filelight_partui.rc filelightui.rc DESTINATION ${DATA_INSTALL_DIR}/filelight)

kde4_install_icons(${ICON_INSTALL_DIR})
