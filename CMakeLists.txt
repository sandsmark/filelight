find_package(KDE4 REQUIRED Qt3Support)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS -fPIC)

include(KDE4Defaults)

include(MacroLibrary)

include_directories(src/ ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(src)
add_subdirectory(misc)
