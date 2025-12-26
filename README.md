## Overview:
This application reads Minecraft world files and scans for player-made bases by detecting certain blocks that are very likely to be found as part of a base (crafting tables, chests, furnaces, etc.), then shows findings on an interactive map.

## Dependencies:
Make sure you have Qt6, Qt VS Tools, vcpkg and CMake installed. CMake will automatically fetch and build other dependencies (libnbtplusplus, zlib) during during the configuration process.