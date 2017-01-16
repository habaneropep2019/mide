cmake_minimum_required (VERSION 3.0)

project (kdetoys)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

find_package(KDE1 REQUIRED)
#find_package(uulib REQUIRED)

add_definitions(-DHAVE_CONFIG_H)

include_directories(common)

include(CMakePackageConfigHelpers)
include(KDE1Macros)
include(KDE1InstallDirs)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif()

add_subdirectory(kmoon)
add_subdirectory(kworldwatch)
add_subdirectory(mouse)
add_subdirectory(po)
