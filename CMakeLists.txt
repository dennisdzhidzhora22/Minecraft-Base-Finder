cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/denni/source/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(MinecraftBaseFinder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/msvc2022_64")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)
FetchContent_Declare(
	libnbtplusplus
	GIT_REPOSITORY https://github.com/PrismLauncher/libnbtplusplus.git
	GIT_TAG	master
)

FetchContent_MakeAvailable(libnbtplusplus)

add_subdirectory("Minecraft Base Finder")
add_subdirectory("BaseFinderView")