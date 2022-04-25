# Translation files
set(TS_FILES QImageAnimator_en_US.ts)

set(PROJECT_SOURCES
    main.cpp
    QImageAnimator.cpp
    QImageAnimator.h
    QImageAnimator.ui
    QClipPlayer.cpp
    QClipPlayer.h
    ScreenAreaSelection.h
    ScreenAreaSelection.cpp
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QImageAnimator
        WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(QImageAnimator
        WIN32
        ${PROJECT_SOURCES}
    )

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(QImageAnimator PRIVATE compiler_flags)
target_link_libraries(QImageAnimator PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QImageAnimator PRIVATE GifH)

set_target_properties(QImageAnimator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QImageAnimator)
endif()


install(TARGETS QImageAnimator DESTINATION bin)


if(TARGET Qt${QT_VERSION_MAJOR}::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET QImageAnimator
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt${QT_VERSION_MAJOR}_install_prefix}/bin
        COMMAND Qt${QT_VERSION_MAJOR}::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" "$<TARGET_FILE_DIR:QImageAnimator>/$<TARGET_FILE_NAME:QImageAnimator>"
    )

    add_custom_command(TARGET QImageAnimator
        POST_BUILD
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt${QT_VERSION_MAJOR}_install_prefix}/bin
        COMMAND Qt${QT_VERSION_MAJOR}::windeployqt "$<TARGET_FILE_DIR:QImageAnimator>/$<TARGET_FILE_NAME:QImageAnimator>"
    )

    # copy deployment directory during installation
    install(
        DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/"
        DESTINATION bin
    )
endif()

install(
    PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION bin
)