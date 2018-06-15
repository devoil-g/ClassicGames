# ClassicGames
A collection of classical games re-coded in C++

## How to build (Windows):

1. Get SFML prebuilt libraries at http://www.sfml-dev.org/download.php
2. Extract SFML library archive in directory \<library\>/SFML-2.x.x
3. Set environment variable SFML_ROOT to \<library\>/SFML-2.x.x
4. Clone repository in directory \<sources\>
5. Create a directory \<build\>
6. CMake-GUI :
  1. Set source code directory to \<sources\>
  2. Set build directory to \<build\>
  3. Configure (it will fail)
  4. Add an entry CMAKE_MODULE_PATH of type PATH with value \<library\>/SFML-2.x.x/cmake/Modules
  5. Configure
  6. Generate
9. Project has been generated in \<build\>

## How tu build (Linux):

You use Linux. You're strong. Do it yourself. Use CMake.