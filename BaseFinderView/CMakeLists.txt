set(SOURCES 
    BaseFinderView.cpp
    main.cpp
    stdafx.cpp
)

set(HEADERS 
    BaseFinderView.h
    ZoomableGraphicsView.h
    stdafx.h
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
    
    # Copy Qt DLLs to output directory (for deployment)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET BaseFinderView POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PATH="${_qt_bin_dir}" 
            "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe" 
            --debug --compiler-runtime "$<TARGET_FILE:BaseFinderView>"
        )
    else()
        add_custom_command(TARGET BaseFinderView POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PATH="${_qt_bin_dir}" 
            "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe" 
            --release --compiler-runtime "$<TARGET_FILE:BaseFinderView>"
        )
    endif()
endif()