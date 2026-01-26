set(SOURCES 
    BaseFinderView.cpp
    main.cpp
    stdafx.cpp
    RegionTask.cpp
)

set(HEADERS 
    BaseFinderView.h
    ZoomableGraphicsView.h
    stdafx.h
    RegionTask.h
)

set(UI_FILES 
    BaseFinderView.ui
)

set(RESOURCE_FILES
    BaseFinderView.qrc
)

add_executable(BaseFinderView ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCE_FILES})

target_link_libraries(BaseFinderView PRIVATE
    MinecraftBaseFinder
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(BaseFinderView PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Windows-specific settings for Qt
if(WIN32)
    # Set as Windows application (not console)
    set_target_properties(BaseFinderView PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    set(WINDEPLOYQT_EXE "$ENV{QTDIR}/bin/windeployqt.exe")
    get_filename_component(QT_BIN_DIR "${WINDEPLOYQT_EXE}" DIRECTORY)
    file(TO_NATIVE_PATH "${QT_BIN_DIR}" QT_BIN_DIR_NATIVE)
    file(TO_NATIVE_PATH "${WINDEPLOYQT_EXE}" WINDEPLOYQT_EXE_NATIVE)

    # Zlib
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ZLIB_DLL_SRC "$ENV{VCPKG_ROOT}/installed/x64-windows/debug/bin/zlibd1.dll")
    else()
        set(ZLIB_DLL_SRC "$ENV{VCPKG_ROOT}/installed/x64-windows/bin/zlib1.dll")
    endif()
    
    # Copy Qt DLLs to output directory (for deployment)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET BaseFinderView POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PATH="${QT_BIN_DIR_NATIVE}" 
            "${WINDEPLOYQT_EXE_NATIVE}" 
            --debug --compiler-runtime "$<TARGET_FILE:BaseFinderView>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ZLIB_DLL_SRC}"
            "$<TARGET_FILE_DIR:BaseFinderView>"
        )
    else()
        add_custom_command(TARGET BaseFinderView POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PATH="${QT_BIN_DIR_NATIVE}" 
            "${WINDEPLOYQT_EXE_NATIVE}" 
            --release --compiler-runtime "$<TARGET_FILE:BaseFinderView>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ZLIB_DLL_SRC}"
            "$<TARGET_FILE_DIR:BaseFinderView>"
        )
    endif()
endif()