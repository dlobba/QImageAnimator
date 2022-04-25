cmake_minimum_required(VERSION 3.15)

project(QImageAnimator VERSION 0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)

# ---------------------------------------
# Manage dependencies
# ---------------------------------------
# Init git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()


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


# Define compiler options target
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)
target_compile_options(compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall$<SEMICOLON>-Wextra$<SEMICOLON>-Wshadow$<SEMICOLON>-Wformat=2$<SEMICOLON>-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

# ---------------------------------------
# App sources
# ---------------------------------------
add_subdirectory(src)
