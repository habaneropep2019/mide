cmake_minimum_required (VERSION 3.0)

project (kdegames)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

find_package(KDE1 REQUIRED)

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

#add_subdirectory(bsd-port)
add_subdirectory(kabalone)
#add_subdirectory(kasteroids) # need kdesupport-1.1.2 for that (QwSpriteField.h)
add_subdirectory(kblackbox)
#add_subdirectory(kenolaba) # Apparently this is the old name for kabalone
add_subdirectory(kmahjongg)
add_subdirectory(kmines)
add_subdirectory(konquest)
add_subdirectory(kpat)
#add_subdirectory(kpoker)
#add_subdirectory(kreversi)
#add_subdirectory(ksame)
#add_subdirectory(kshisen)
#add_subdirectory(ksirtet)
#add_subdirectory(ksnake)

add_subdirectory(po)
