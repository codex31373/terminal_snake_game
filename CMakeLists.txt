cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(BOX2D REQUIRED box2d)

add_executable(snake src/main.cpp src/render_engine.cpp src/game.cpp)

add_executable(physics src/physics.cpp src/render_engine.cpp)
target_include_directories(physics PRIVATE ${BOX2D_INCLUDE_DIRS})
target_link_libraries(physics ${BOX2D_LIBRARIES})
target_compile_options(physics PRIVATE ${BOX2D_CFLAGS_OTHER})
