cmake_minimum_required(VERSION 3.25)
project(ActiveMap)

set(SRC "${CMAKE_SOURCE_DIR}/src")

find_package(wxWidgets REQUIRED)

include_directories(${PROJECT_NAME} PRIVATE ${SRC})

file(GLOB_RECURSE SRC_FILES "${SRC}/*.hpp" "${SRC}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

source_group("src" FILES ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} PRIVATE wxWidgets::wxWidgets)