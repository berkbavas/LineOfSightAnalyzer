# Line of Sight Analyzer
This line of sight analyzer is implemented without using ray casting techniques.
Instead I used omnidirectional shadow mapping idea in order to generate line of sight (LOS) map.
The application runs at 144 FPS (V-Sync enabled) on my PC.

## Build
1) Install `CMake 3.25.1` or above.
2) Install `Visual Studio 2022` and `MSVC C++ v143 Compiler`.
3) Install `Qt 5.x.y MSVC2019 64bit` kit.
4) Set environment variable `Qt5_DIR` as `C:\Qt\5.x.y\msvc2019_64`.
5) Clone the repo `git clone https://github.com/berkbavas/LineOfSightAnalyzer.git`.
6) Create a folder `mkdir Build`.
7) Enter the folder `cd Build`.
8) Run CMake `cmake ..`.
9) Open `LineOfSightAnalyzer.sln` with `Visual Studio 2022`.
10) Build & Run with `Release` config.

## Screenshots

![image](https://user-images.githubusercontent.com/53399385/201484129-67f08fa8-5f10-4456-af9e-e9180bfe404d.png)

![image](https://user-images.githubusercontent.com/53399385/201484169-9099b497-c876-4145-8332-6c7a50e5621d.png)
