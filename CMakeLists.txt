cmake_minimum_required(VERSION 3.22)

project(spiralfun VERSION 4.1 LANGUAGES CXX)
set(VERSION_CODE 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Work around to fix build issue in Qt6.8.2
# See https://bugreports.qt.io/browse/QTBUG-131707
set(QT_NO_GLOBAL_APK_TARGET_PART_OF_ALL OFF)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickControls2 Core Multimedia)

if (ANDROID AND Qt6Core_VERSION VERSION_GREATER_EQUAL 6.9.0)
    find_package(Qt6 REQUIRED COMPONENTS CorePrivate)
endif()

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

set(PROJECT_SOURCES
        circle.h
        circle.cpp
        display_utils.h
        display_utils.cpp
        enums.h
        exception.h
        flash.h
        flash.cpp
        gif_encoder_wrapper.h
        gif_encoder_wrapper.cpp
        jni_callback.h
        jni_callback.cpp
        main.cpp
        music_generator.h
        music_generator.cpp
        mutation.h
        mutation.cpp
        mutation_sequence.h
        mutation_sequence.cpp
        player.h
        player.cpp
        recorder.h
        recorder.cpp
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
        video_encoder.h
        video_encoder.cpp
        video_encoder_interface.h
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
        GuiSettings.qml

        main.qml
        message.qml
        MusicDialog.qml
	MutationSequenceDialog.qml
    RESOURCES
        "android/src/com/gmail/mfnboer/QAndroidUtils.java"
        "android/src/com/gmail/mfnboer/QSpiralFunActivity.java"
        "android/src/com/gmail/mfnboer/ScreenUtils.java"
        "android/src/com/gmail/mfnboer/SpiralFunApplication.java"
        "android/src/com/gmail/mfnboer/QVideoEncoder.java"
        "icons/spiralfun/20x20/arrow_downward.png"
        "icons/spiralfun/20x20/arrow_upward.png"
        "icons/spiralfun/20x20/delete.png"
        "icons/spiralfun/20x20/home.png"
        "icons/spiralfun/20x20/more.png"
        "icons/spiralfun/20x20/music.png"
        "icons/spiralfun/20x20/music_off.png"
        "icons/spiralfun/20x20/play.png"
        "icons/spiralfun/20x20/share.png"
        "icons/spiralfun/20x20/stop.png"
        "icons/spiralfun/20x20@2/arrow_downward.png"
        "icons/spiralfun/20x20@2/arrow_upward.png"
        "icons/spiralfun/20x20@2/delete.png"
        "icons/spiralfun/20x20@2/home.png"
        "icons/spiralfun/20x20@2/more.png"
        "icons/spiralfun/20x20@2/music.png"
        "icons/spiralfun/20x20@2/music_off.png"
        "icons/spiralfun/20x20@2/play.png"
        "icons/spiralfun/20x20@2/share.png"
        "icons/spiralfun/20x20@2/stop.png"
        "icons/spiralfun/20x20@3/arrow_downward.png"
        "icons/spiralfun/20x20@3/arrow_upward.png"
        "icons/spiralfun/20x20@3/delete.png"
        "icons/spiralfun/20x20@3/home.png"
        "icons/spiralfun/20x20@3/more.png"
        "icons/spiralfun/20x20@3/music.png"
        "icons/spiralfun/20x20@3/music_off.png"
        "icons/spiralfun/20x20@3/play.png"
        "icons/spiralfun/20x20@3/share.png"
        "icons/spiralfun/20x20@3/stop.png"
        "icons/spiralfun/20x20@4/arrow_downward.png"
        "icons/spiralfun/20x20@4/arrow_upward.png"
        "icons/spiralfun/20x20@4/delete.png"
        "icons/spiralfun/20x20@4/home.png"
        "icons/spiralfun/20x20@4/more.png"
        "icons/spiralfun/20x20@4/music.png"
        "icons/spiralfun/20x20@4/music_off.png"
        "icons/spiralfun/20x20@4/play.png"
        "icons/spiralfun/20x20@4/share.png"
        "icons/spiralfun/20x20@4/stop.png"
        "icons/spiralfun/index.theme"
        "images/example1.png"
        "images/example2.png"
        "images/example3.png"
        "images/example4.png"
        "images/question_mark.png"
        "images/kofi_button_blue.webp"
        "piano-wav/A2.wav"
        "piano-wav/Ab2.wav"
        "piano-wav/B2.wav"
        "piano-wav/Bb2.wav"
        "piano-wav/C2.wav"
        "piano-wav/D2.wav"
        "piano-wav/Db2.wav"
        "piano-wav/E2.wav"
        "piano-wav/Eb2.wav"
        "piano-wav/F2.wav"
        "piano-wav/G2.wav"
        "piano-wav/Gb2.wav"
        "piano-wav/A3.wav"
        "piano-wav/Ab3.wav"
        "piano-wav/B3.wav"
        "piano-wav/Bb3.wav"
        "piano-wav/C3.wav"
        "piano-wav/D3.wav"
        "piano-wav/Db3.wav"
        "piano-wav/E3.wav"
        "piano-wav/Eb3.wav"
        "piano-wav/F3.wav"
        "piano-wav/G3.wav"
        "piano-wav/Gb3.wav"
        "piano-wav/A4.wav"
        "piano-wav/Ab4.wav"
        "piano-wav/B4.wav"
        "piano-wav/Bb4.wav"
        "piano-wav/C4.wav"
        "piano-wav/D4.wav"
        "piano-wav/Db4.wav"
        "piano-wav/E4.wav"
        "piano-wav/Eb4.wav"
        "piano-wav/F4.wav"
        "piano-wav/G4.wav"
        "piano-wav/Gb4.wav"
        "piano-wav/A5.wav"
        "piano-wav/Ab5.wav"
        "piano-wav/B5.wav"
        "piano-wav/Bb5.wav"
        "piano-wav/C5.wav"
        "piano-wav/D5.wav"
        "piano-wav/Db5.wav"
        "piano-wav/E5.wav"
        "piano-wav/Eb5.wav"
        "piano-wav/F5.wav"
        "piano-wav/G5.wav"
        "piano-wav/Gb5.wav"
        "piano-wav/A6.wav"
        "piano-wav/Ab6.wav"
        "piano-wav/B6.wav"
        "piano-wav/Bb6.wav"
        "piano-wav/C6.wav"
        "piano-wav/D6.wav"
        "piano-wav/Db6.wav"
        "piano-wav/E6.wav"
        "piano-wav/Eb6.wav"
        "piano-wav/F6.wav"
        "piano-wav/G6.wav"
        "piano-wav/Gb6.wav"
        "qtquickcontrols2.conf"
)

set_target_properties(spiralfun PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    QT_ANDROID_TARGET_SDK_VERSION 35
    QT_ANDROID_SDK_BUILD_TOOLS_REVISION 35.0.0
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
    Qt${QT_VERSION_MAJOR}::Multimedia
    egif
)

if (ANDROID AND Qt6Core_VERSION VERSION_GREATER_EQUAL 6.9.0)
    target_link_libraries(spiralfun
        PRIVATE Qt6::CorePrivate
    )
endif()

set_target_properties(spiralfun PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(spiralfun PROPERTIES
        QT_ANDROID_ABIS "arm64-v8a;armeabi-v7a"
    )
else()
    set_target_properties(spiralfun PROPERTIES
        QT_ANDROID_ABIS "arm64-v8a"
    )
endif()

install(TARGETS spiralfun
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(spiralfun)
