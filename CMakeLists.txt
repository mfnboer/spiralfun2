cmake_minimum_required(VERSION 3.5)

project(spiralfun VERSION 2.2 LANGUAGES CXX)
set(VERSION_CODE 4)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickControls2 Core)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

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
        mutation.h
        mutation.cpp
        mutation_sequence.h
        mutation_sequence.cpp
        player.h
        player.cpp
        scene_grabber.h
        scene_grabber.cpp
        scoped_line.h
        scoped_line.cpp
        spiral_config.h
        spiral_config.cpp
        spiral_scene.h
        spiral_scene.cpp
        utils.h
        utils.cpp
)

set_source_files_properties(
    ${PROJECT_SOURCES}
    PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror"
)

qt_add_executable(spiralfun
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

qt_add_qml_module(spiralfun
    URI SpiralFun
    VERSION ${PROJECT_VERSION}
    NO_RESOURCE_TARGET_PATH
    QML_FILES
        about.qml
        color_selector.qml
        examples.qml
        file_dialog.qml
        help.qml
        main.qml
        message.qml
	MutationSequenceDialog.qml
    RESOURCES
        "android/src/com/gmail/mfnboer/QAndroidUtils.java"
        "android/src/com/gmail/mfnboer/QSpiralFunActivity.java"
        "icons/spiralfun/20x20/arrow_downward.png"
        "icons/spiralfun/20x20/arrow_upward.png"
        "icons/spiralfun/20x20/delete.png"
        "icons/spiralfun/20x20/home.png"
        "icons/spiralfun/20x20/more.png"
        "icons/spiralfun/20x20/play.png"
        "icons/spiralfun/20x20/share.png"
        "icons/spiralfun/20x20/stop.png"
        "icons/spiralfun/20x20@2/arrow_downward.png"
        "icons/spiralfun/20x20@2/arrow_upward.png"
        "icons/spiralfun/20x20@2/delete.png"
        "icons/spiralfun/20x20@2/home.png"
        "icons/spiralfun/20x20@2/more.png"
        "icons/spiralfun/20x20@2/play.png"
        "icons/spiralfun/20x20@2/share.png"
        "icons/spiralfun/20x20@2/stop.png"
        "icons/spiralfun/20x20@3/arrow_downward.png"
        "icons/spiralfun/20x20@3/arrow_upward.png"
        "icons/spiralfun/20x20@3/delete.png"
        "icons/spiralfun/20x20@3/home.png"
        "icons/spiralfun/20x20@3/more.png"
        "icons/spiralfun/20x20@3/play.png"
        "icons/spiralfun/20x20@3/share.png"
        "icons/spiralfun/20x20@3/stop.png"
        "icons/spiralfun/20x20@4/arrow_downward.png"
        "icons/spiralfun/20x20@4/arrow_upward.png"
        "icons/spiralfun/20x20@4/delete.png"
        "icons/spiralfun/20x20@4/home.png"
        "icons/spiralfun/20x20@4/more.png"
        "icons/spiralfun/20x20@4/play.png"
        "icons/spiralfun/20x20@4/share.png"
        "icons/spiralfun/20x20@4/stop.png"
        "icons/spiralfun/index.theme"
        "images/example1.png"
        "images/example2.png"
        "images/example3.png"
        "images/example4.png"
        "images/question_mark.png"
        "qtquickcontrols2.conf"
)

set_target_properties(spiralfun PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    QT_ANDROID_TARGET_SDK_VERSION 31
    QT_ANDROID_VERSION_NAME ${PROJECT_VERSION}
    QT_ANDROID_VERSION_CODE ${VERSION_CODE})

# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

file(GLOB_RECURSE EGIF_SOURCES "egif/*.h" "egif/*.cpp")
add_library(egif STATIC ${EGIF_SOURCES})

target_link_libraries(spiralfun PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Core
    egif
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

qt_finalize_executable(spiralfun)
