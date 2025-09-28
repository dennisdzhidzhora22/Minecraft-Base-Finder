file(GLOB_RECURSE SOURCES 
    "*.cpp" 
    "*.c"
)
file(GLOB_RECURSE HEADERS 
    "*.h" 
    "*.hpp"
)

add_library(MinecraftBaseFinder STATIC ${SOURCES} ${HEADERS})

find_package(ZLIB REQUIRED)

target_link_libraries(MinecraftBaseFinder PUBLIC
    nbt++
	ZLIB::ZLIB
)

target_include_directories(MinecraftBaseFinder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)