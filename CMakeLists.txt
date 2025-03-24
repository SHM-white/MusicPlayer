cmake_minimum_required(VERSION 3.16)

project(MusicPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(QWINDOWKIT_BUILD_EXAMPLES ON CACHE BOOL "")
set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "")

set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "E:/Qt/6.8.2/msvc2022_64")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/QWindowKit)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgetframe)

set(TS_FILES TSFiles/MusicPlayer_zh_CN.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/widget.cpp
        src/widget.h
        src/widget.ui
        src/BasicWidget.cpp
        src/BasicWidget.h
        src/includeFiles.h
        src/ProgressBar.h
        src/ProgressBar.cpp
        src/PlayPauseButton.h
        src/PlayPauseButton.cpp
        # ${TS_FILES}
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
    # qt_create_translation(QM_FILES ${TS_FILES} ${PROJECT_SOURCES})
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

    # 修复翻译文件生成路径
    # qt_create_translation(QM_FILES ${TS_FILES} ${PROJECT_SOURCES})
endif()

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
qt_create_translation(QM_FILES ${TS_FILES} ${PROJECT_SOURCES})
add_custom_target(lupdate_task DEPENDS ${TS_FILES})
add_custom_target(lrelease_task DEPENDS ${QM_FILES})
qt_add_resources(${PROJECT_NAME} ${QM_FILES})

target_link_libraries(MusicPlayer PRIVATE 
Qt${QT_VERSION_MAJOR}::Widgets 
QWindowKit::Widgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MusicPlayer)
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
