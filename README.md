# Minecraft Base Finder

## Overview
This application reads Minecraft world files and scans for player-made bases by detecting certain blocks that are very likely to be found as part of a base (crafting tables, chests, furnaces, etc.), then shows findings on an interactive map.

## Dependencies
### Note: Currently only Windows is supported.
* **Qt6** - Version 6.8.2 (MSVC 2022 64-bit) or newer. I recommend using a standalone installation from the [Qt Online Installer](https://doc.qt.io/qt-6/get-and-install-qt.html) to avoid the lengthy build time required when installing through vcpkg. Additional Libraries in the installer can be excluded.
* **Qt VS Tools (optional)** - Follow steps 1 and 2 [here](https://doc.qt.io/qtvstools/qtvstools-getting-started.html).
* **vcpkg** - Follow steps 1 and 2 [here](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell), and make sure to set the environment variables through the Windows System Environment Variables panel.
* **CMake** - Might be bundled with Visual Studio, but if not, you can get it from the Windows x64 installer [here](https://cmake.org/download/). Make sure to check the option to add CMake to PATH.
* **libnbt++** and **Zlib** - libnbt++ will be installed during CMake configuration. Zlib is installed in step 3 of the building instructions below.

## Building
### Visual Studio
1. Clone repository.
2. Close Visual Studio and make a copy of CMakeUserPresets.template.json, then rename the copy to "CMakeUserPresets.json". Find the VCPKG_ROOT and QTDIR environment variables in CMakeUserPresets.json and set each to their respective paths as shown in the examples, making sure the paths use forward slashes, not backslashes.
3. Run "vcpkg install zlib:x64-windows" in command prompt to install Zlib.
4. Open Visual Studio again. If CMake configuration ran automatically, click Delete Cache and Reconfigure (found in Project menu). If not, click Configure Cache in the same menu. Then, click Build All and run.