cmake_minimum_required(VERSION 3.7)

project(play C)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(SDL2_GFX SDL2_gfx)
include_directories(${SDL2_GFX_INCLUDE_DIRS})

# Add SDL2_ttf
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
include_directories(${SDL2_TTF_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall -Werror -fdump-rtl-expand")

set(SRCS
    main.c
    logic.c
    rendering.c
    menu.c
    ai.c
    snake.c
    window.c
    score.c
)

set(HEADERS
    logic.h
    rendering.h
    game.h
    menu.h
    ai.h
    snake.h
    window.h
    score.h
)

add_executable(play ${SRCS} ${HEADERS})
target_link_libraries(play 
    ${SDL2_LIBRARIES} 
    ${SDL2_GFX_LIBRARIES} 
    ${SDL2_TTF_LIBRARIES}
    m
)