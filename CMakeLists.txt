cmake_minimum_required(VERSION 3.5)

project(spiralfun VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickControls2)

add_compile_options(-Wall -Wextra -Werror)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

set(PROJECT_SOURCES
#	appwindow.h
#	appwindow.cpp
        circle.h
        circle.cpp
#        color_select_button.h
#        color_select_button.cpp
#        graphics_view.h
#        graphics_view.cpp
#        image_button.h
        main.cpp
        player.h
        player.cpp
#        spinbox.h
        spiral_scene.h
        spiral_scene.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(spiralfun
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_add_qml_module(spiralfun
        URI SpiralFun
        VERSION 1.0
        QML_FILES
            about.qml
            color_selector.qml
            main.qml
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
# Define properties for Android with Qt 5 after find_package() calls as:
#        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
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
)

set_target_properties(spiralfun PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_ABIS "arm64-v8a;armeabi-v7a"
)

qt_add_resources(spiralfun "images"
    PREFIX "/"
    FILES
        images/appicon.png
        images/example1.png
        images/example2.png
        images/example3.png
        images/example4.png
)

install(TARGETS spiralfun
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(spiralfun)
endif()
