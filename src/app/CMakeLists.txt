include_directories(${CMAKE_SOURCE_DIR}/filelight/src ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

set(filelight_SRCS historyAction.cpp mainWindow.cpp main.cpp)

kde4_add_executable(filelight ${filelight_SRCS})

target_link_libraries(filelight ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS filelight ${INSTALL_TARGETS_DEFAULT_ARGS})
