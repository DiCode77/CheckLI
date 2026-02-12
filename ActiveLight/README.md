# ActiveLight

ActiveLight is a program for processing and structuring incoming information. It has methods that help you obtain the necessary information and, if necessary, supplement or edit it.  

In fact, this project is the basis for the development of other projects, as it will be used to obtain information about the availability of light.

## Requirements
- c++23
- CMake >= 3.25
- Conan 2

## Technologies used

- CMake - For automation of the assembly system.
- Conan - C++ Package Manager (To attach the necessary libraries).


## Installation

To use it, you need to have Cmake and Conan already installed!.

Then enter the commands: 

```bash
# First, check whether the necessary tools are installed. 
cmake --version
conan --version

# Let's move on to the installation.
git clone https://github.com/DiCode77/CheckLI.git
cd ActiveLight

conan profile detect # Performed only once after installation

mkdir build
cd build

conan install .. --output-folder=. --build=missing -s build_type=Release

# Important note: first, run the cmake -G command and select your build system. For me, it is “Unix Makefiles.”
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake  --build .


# Launching the program
 ./ActiveLight

# After launching, the program will display a list of all cities where fireflies are found, which means that the compilation was successful.


```
