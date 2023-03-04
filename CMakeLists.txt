cmake_minimum_required(VERSION 3.5)

project(spiralfun VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickControls2 Core)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")
add_compile_options(-Wall -Wextra -Werror)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

set(PROJECT_SOURCES
        circle.h
        circle.cpp
        exception.h
        jni_callback.h
        jni_callback.cpp
        main.cpp
        player.h
        player.cpp
        spiral_config.h
        spiral_config.cpp
        spiral_scene.h
        spiral_scene.cpp
        utils.h
        utils.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(spiralfun
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_add_qml_module(spiralfun
        URI SpiralFun
        VERSION 1.0
        NO_RESOURCE_TARGET_PATH
        QML_FILES
            about.qml
            color_selector.qml
            examples.qml
            file_dialog.qml
            help.qml
            main.qml
            message.qml
        RESOURCES
            "android/src/com/gmail/mfnboer/QAndroidUtils.java"
            "android/src/com/gmail/mfnboer/QSpiralFunActivity.java"
            "icons/spiralfun/20x20/arrow_downward.png"
            "icons/spiralfun/20x20/arrow_upward.png"
            "icons/spiralfun/20x20/home.png"
            "icons/spiralfun/20x20/more.png"
            "icons/spiralfun/20x20/play.png"
            "icons/spiralfun/20x20/share.png"
            "icons/spiralfun/20x20/stop.png"
            "icons/spiralfun/20x20@2/arrow_downward.png"
            "icons/spiralfun/20x20@2/arrow_upward.png"
            "icons/spiralfun/20x20@2/home.png"
            "icons/spiralfun/20x20@2/more.png"
            "icons/spiralfun/20x20@2/play.png"
            "icons/spiralfun/20x20@2/share.png"
            "icons/spiralfun/20x20@2/stop.png"
            "icons/spiralfun/20x20@3/arrow_downward.png"
            "icons/spiralfun/20x20@3/arrow_upward.png"
            "icons/spiralfun/20x20@3/home.png"
            "icons/spiralfun/20x20@3/more.png"
            "icons/spiralfun/20x20@3/play.png"
            "icons/spiralfun/20x20@3/share.png"
            "icons/spiralfun/20x20@3/stop.png"
            "icons/spiralfun/20x20@4/arrow_downward.png"
            "icons/spiralfun/20x20@4/arrow_upward.png"
            "icons/spiralfun/20x20@4/home.png"
            "icons/spiralfun/20x20@4/more.png"
            "icons/spiralfun/20x20@4/play.png"
            "icons/spiralfun/20x20@4/share.png"
            "icons/spiralfun/20x20@4/stop.png"
            "icons/spiralfun/index.theme"
            "images/appicon.png"
            "images/example1.png"
            "images/example2.png"
            "images/example3.png"
            "images/example4.png"
            "images/question_mark.png"
            "qtquickcontrols2.conf"
    )

   set_property(TARGET spiralfun APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
   set_property(TARGET spiralfun APPEND PROPERTY QT_ANDROID_TARGET_SDK_VERSION 31)

# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(spiralfun SHARED
            ${PROJECT_SOURCES}
        )
        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(spiralfun
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(spiralfun PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(spiralfun PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_ABIS "arm64-v8a;armeabi-v7a"
)

install(TARGETS spiralfun
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(spiralfun)
endif()
