cmake_minimum_required(VERSION 3.5)

project(MCORD_GUI_CPP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/Qt/6.5.2/gcc_64/lib/cmake")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort Network Sql Svg)
find_package(GSL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        widget.h
        widget.cpp
        slab.h
        slab.cpp
        lanconnection.h
        lanconnection.cpp
        sipm.h
        sipm.cpp
        afetype.h
        statuscolor.h
        detectortablemodel.h
        detectortablemodel.cpp
        statusicondelegate.h
        statusicondelegate.cpp
        manyslabsatonce.h
        manyslabsatonce.cpp
        images.qrc
        widget.ui
        manyslabsatonce.ui
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MCORD_GUI_CPP
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        manyslabsatonce.h manyslabsatonce.cpp manyslabsatonce.ui
        basewidget.h basewidget.cpp
        wizard.h wizard.cpp wizard.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MCORD_GUI_CPP APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MCORD_GUI_CPP SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MCORD_GUI_CPP
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MCORD_GUI_CPP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(MCORD_GUI_CPP PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(MCORD_GUI_CPP PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(MCORD_GUI_CPP PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(MCORD_GUI_CPP PRIVATE GSL::gsl GSL::gslcblas)

set_target_properties(MCORD_GUI_CPP PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS MCORD_GUI_CPP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MCORD_GUI_CPP)
endif()
