cmake_minimum_required(VERSION 3.20)

project(powder_playground)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/raylib-4.2.0)
add_subdirectory(lib/raylib-cpp-4.2.7)

set(LIB_INCLUDES
        lib/raygui-3.2/include
        )

set(SOURCE_FILES
        src/main.cpp
        src/fixed_loop.cpp
        src/powder_playground.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${LIB_INCLUDES})

target_link_libraries(${PROJECT_NAME} raylib raylib_cpp)