cmake_minimum_required(VERSION 3.27)
project(TgBot)

set(SRC "${CMAKE_SOURCE_DIR}/src")

include_directories(${PROJECT_NAME} PRIVATE ${SRC})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ActiveLight ${CMAKE_CURRENT_BINARY_DIR}/ActiveLight)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/TGbot_cpp ${CMAKE_CURRENT_BINARY_DIR}/TGbot_cpp)

file(GLOB_RECURSE SRC_FILES "${SRC}/*.hpp" "${SRC}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

source_group("src" FILES ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} PRIVATE ActiveLightLib TGbotCppLib)