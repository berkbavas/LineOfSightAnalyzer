# Line of Sight Analyzer

A high-performance line of sight (LOS) analysis tool using omnidirectional shadow mapping, built with C++, Qt 6, and OpenGL.
This project avoids traditional ray casting, leveraging GPU-based techniques for real-time LOS visualization on terrain heightmaps.

## Getting Started

### Prerequisites

- **CMake** >= 3.25.1
- **Visual Studio 2022** with MSVC v143
- **Qt 6.7.3** (MSVC2022 64bit)

### Build Instructions

1. Install the prerequisites above.
2. Set the environment variable `Qt6_DIR` to your Qt installation, e.g.:

 ```
 set Qt6_DIR=C:\Qt\6.7.3\msvc2022_64
 ```

3. Clone the repository:

 ```
 git clone https://github.com/berkbavas/LineOfSightAnalyzer.git
 ```

4. Create and enter a build directory:

 ```
 mkdir build
 cd build
 ```

5. Run CMake:

 ```
 cmake ..
 ```

6. Open `LineOfSightAnalyzer.sln` in Visual Studio 2022.
7. Build and run the project in **Release** mode.

## Demo

[Demo Video](https://github.com/user-attachments/assets/ff979701-34ea-4a09-b14c-b6a6bd0ede57)

## License

This project is licensed under the [MIT License](LICENSE).
