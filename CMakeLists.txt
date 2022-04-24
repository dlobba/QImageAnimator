cmake_minimum_required(VERSION 3.5)

project(QImageAnimator VERSION 0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------------------------------------
# Manage dependencies
# ---------------------------------------
# system dependencies
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

# local dependencies
add_subdirectory(extern/gif-h)

# ---------------------------------------
# App sources
# ---------------------------------------
add_subdirectory(src)


