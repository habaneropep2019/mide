#=============================================================================
# Copyright 2016 Helio Chissini de Castro <helio@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set(QT2_FOUND 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(Qt2Macros)

if(UNIX)
  find_path(QT_INCLUDE_DIR qapp.h
    /opt/qt2/include
    /opt/qt2/include/qt
    ${QT2_INCLUDE_DIR}
    )

  find_library(QT_LIBRARIES libqt
    /opt/qt2/lib64
    /opt/qt2/lib
    ${QT2_LIBRARY_DIR}
    )

endif()

# handle the QUIETLY and REQUIRED arguments and set MOTIF_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Qt2 DEFAULT_MSG QT_LIBRARIES QT_INCLUDE_DIR)

mark_as_advanced(
  QT_INCLUDE_DIR
  QT_LIBRARIES
)
