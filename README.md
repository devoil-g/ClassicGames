# ClassicGames
A collection of classical games re-coded in C++

## How to build (Windows):

1. Get SFML prebuilt libraries at http://www.sfml-dev.org/download.php
2. Extract SFML library archive in directory \<library\>/SFML-2.x.x
3. Clone repository in directory \<sources\>
4. Create a directory \<build\>
5. CMake-GUI :
   1. Set source code directory to \<sources\>
   2. Set build directory to \<build\>
   3. Configure (it will fail)
   4. Set CMake entry SFML_DIR path value to \<library\>/SFML-2.x.x/lib/cmake/SFML
   5. Configure
   6. Generate
6. Project has been generated in \<build\>
7. When project has been built, copy \<library\>/SFML-2.x.x/bin/openal32.dll next to game executable

## How tu build (Linux):

You use Linux. You're strong. Do it yourself. Use CMake.