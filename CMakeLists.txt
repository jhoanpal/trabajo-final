cmake_minimum_required(VERSION 3.16)

project(proyecto_final LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

add_executable(proyecto_final 
    main.cpp
    character.cpp character.h
    projectile.cpp projectile.h
    piccolo.cpp piccolo.h
    gameview.cpp gameview.h
)

target_link_libraries(proyecto_final PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)


