cmake_minimum_required(VERSION 3.5)

project(QImageAnimator VERSION 0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)

# ---------------------------------------
# Manage dependencies
# ---------------------------------------
include(InstallRequiredSystemLibraries)
# system dependencies
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

# local dependencies
add_subdirectory(extern/gif-h)

# ---------------------------------------
# Define Qt Windeploy: https://stackoverflow.com/questions/41193584/deploy-all-qt-dependencies-when-building
# ---------------------------------------
if(Qt${QT_VERSION_MAJOR}_FOUND AND WIN32 AND TARGET Qt${QT_VERSION_MAJOR}::qmake AND NOT TARGET Qt${QT_VERSION_MAJOR}::windeployqt)
    get_target_property(_qt${QT_VERSION_MAJOR}_qmake_location Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt${QT_VERSION_MAJOR}_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt${QT_VERSION_MAJOR}_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt${QT_VERSION_MAJOR}_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt${QT_VERSION_MAJOR}::windeployqt IMPORTED)

        set_target_properties(Qt${QT_VERSION_MAJOR}::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    endif()
endif()


# ---------------------------------------
# App sources
# ---------------------------------------
add_subdirectory(src)
