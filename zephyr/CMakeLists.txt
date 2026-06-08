cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(nrf_start)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE
    ${app_sources}
    ${font_sources}
)

set(font_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/fonts/font_montserrat_bold_72.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../src/fonts/font_montserrat_bold_48.c
)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/fonts
)
# target_sources(app PRIVATE src/fonts/montserrat_72.c)
