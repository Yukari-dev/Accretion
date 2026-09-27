cmake_minimum_required(VERSION 3.20)
project(Accretion VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL3 REQUIRED)

find_program(GLSLANG_VALIDATOR glslangValidator HINTS ${VULKAN_SDK}/bin)
if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! make sure Vulkan SDK is installed or glslangValidator is in PATH.")
endif()

file(GLOB SHADER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)

set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SPIRV_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_OUTPUT "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SPIRV_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling SPIR-V shader: ${FILE_NAME} -> ${FILE_NAME}.spv"
        VERBATIM
    )
    list(APPEND SPIRV_OUTPUTS ${SPIRV_OUTPUT})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})

file(GLOB_RECURSE ACCRETION_SOURCES
    "src/vulkan_core/*.cpp"
    "src/core/*.cpp"
    "third_party/*c"
)

add_library(Accretion STATIC ${ACCRETION_SOURCES})

target_include_directories(Accretion PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vma
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spirv-reflect
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_definitions(Accretion PUBLIC
  SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)

target_link_libraries(Accretion PUBLIC Vulkan::Vulkan glm glfw SDL3::SDL3)
add_dependencies(Accretion CompileShaders)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sandbox/main.cpp")
    add_executable(accretion sandbox/main.cpp)
    target_link_libraries(accretion PRIVATE Accretion)
endif()
