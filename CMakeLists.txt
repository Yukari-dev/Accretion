cmake_minimum_required(VERSION 3.20)
project(Accretion VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE ACCRETION_SOURCES
    "src/core/*.cpp"
)

add_library(Accretion STATIC ${ACCRETION_SOURCES})

target_include_directories(Accretion PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(Accretion PUBLIC Vulkan::Vulkan glm glfw)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sandbox/main.cpp")
    add_executable(accretion sandbox/main.cpp)
    target_link_libraries(accretion PRIVATE Accretion)
endif()
