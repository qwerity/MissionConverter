cmake_minimum_required(VERSION 3.5)

project(MissionConverter LANGUAGES CXX)

# Common build configs #################################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
# Try to find Qt6 first
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets QUIET)

if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    # If Qt6 is not found, fall back to Qt5
    find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
endif()

########################################################################################################################
add_executable(${PROJECT_NAME}
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  defines.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets)
