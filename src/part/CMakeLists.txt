include_directories(${CMAKE_SOURCE_DIR}/filelight/src ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(radialMap)

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

kde4_add_plugin(filelightpart ${filelight_PART_SRCS})

target_link_libraries(filelightpart radialmap
                                ${KDE4_KDECORE_LIBS}
                                ${KDE4_KPARTS_LIBS}
                                ${KDE4_KDEUI_LIBS}
                                ${KDE4_KDE3SUPPORT_LIBS}
                                ${KDE4_KIO_LIBS}
                                ${KDE4_KFILE_LIBS}
                                qimageblitz)

install(TARGETS filelightpart DESTINATION ${PLUGIN_INSTALL_DIR})
