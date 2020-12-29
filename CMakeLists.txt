cmake_minimum_required(VERSION 3.8)
project(PIMonitor)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
add_definitions(${GTK2_CFLAGS_OTHER})

add_executable(PIMonitor main.c)
target_link_libraries(PIMonitor ${GTK2_LIBRARIES})
