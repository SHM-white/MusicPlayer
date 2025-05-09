﻿cmake_minimum_required(VERSION 3.18)

project(MusicPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "")
set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools Core5Compat Multimedia Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Multimedia Core5Compat Svg)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/QWindowKit)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgetframe)

list(APPEND TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/MusicPlayer_zh_CN.ts)
set_source_files_properties(${TS_FILES}
    PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PROJECT_SOURCES
        src/main.cpp
        src/widget.cpp
        src/widget.h
        src/widget.ui
        src/BasicWidget.cpp
        src/BasicWidget.h
        src/includeFiles.h
        src/PlayPauseButton.h
        src/PlayPauseButton.cpp
        src/ChangeVolumeButton.h
        src/ChangeVolumeButton.cpp
        src/ConfigManager.h
        src/ConfigManager.cpp
        src/MusicItem.h
        src/MusicItem.cpp
        src/DisplayWidget.h
        src/DisplayWidget.cpp
        src/ScrollingLabel.h
        src/ScrollingLabel.cpp
        src/LoopModeSwitcher.h
        src/LoopModeSwitcher.cpp
        src/LyricsDelegate.h
        src/LyricsDelegate.cpp
        src/Settings.h
        src/Settings.cpp
        src/Settings.ui
        src/ChangePlaybackSpeedButton.cpp
        src/ChangePlaybackSpeedButton.h
        ${TS_FILES}
        )
# qt_create_translation(QM_FILES ${TS_FILES} ${PROJECT_SOURCES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MusicPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        skin/skin.qrc
        skin/shared.qrc
    )
    # Define target properties for Android with Qt 6 as:
#    set_property(TARGET MusicPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    # 修复翻译文件生成路径
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(MusicPlayer SHARED
            ${PROJECT_SOURCES}
            skin/skin.qrc
            skin/shared.qrc
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MusicPlayer
            ${PROJECT_SOURCES}
            skin/skin.qrc
            skin/shared.qrc
        )
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

add_custom_target(lupdate_task ALL DEPENDS ${TS_FILES} COMMAND true || echo "lupdate failed, ignoring.")
add_custom_target(lrelease_task ALL DEPENDS ${QM_FILES} COMMAND true || echo "lrelease failed, ignoring.")

target_link_libraries(MusicPlayer PRIVATE 
Qt${QT_VERSION_MAJOR}::Widgets 
QWindowKit::Widgets
Qt6::Multimedia
Qt6::Core5Compat
Qt6::Svg
)

# qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.shmwhite.MusicPlayer)
endif()
set_target_properties(MusicPlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qm_configure_target(${CMAKE_PROJECT_NAME}
    QT_LINKS Widgets
    LINKS QWKCore QWKWidgets WidgetFrame
)

include(GNUInstallDirs)
install(TARGETS MusicPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MusicPlayer)
endif()
