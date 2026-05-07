cmake_minimum_required(VERSION 3.16)

project(I_am_nailong VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/mywindow.cpp
    src/mywindow_logic.cpp
    src/mywindow_physics.cpp
    src/mywindow_collision.cpp
    src/leveldata.cpp

    include/mywindow.h
    include/gameobjects.h
    include/leveldata.h

    resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(I_am_nailong
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(I_am_nailong
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(I_am_nailong PRIVATE include)

target_link_libraries(I_am_nailong PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

include(GNUInstallDirs)
install(TARGETS I_am_nailong
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(I_am_nailong)
endif()