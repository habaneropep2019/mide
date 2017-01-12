add_subdirectory(data)

include_directories(
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KMINES_SRCS
    dialogs.cpp field.cpp status.cpp main.cpp
)

set(KMINES_MOC
    dialogs.moc field.moc status.moc main.moc
)

QT1_WRAP_MOC(MOC_FILES ${KMINES_MOC})

add_executable(kmines ${KMINES_SRCS} ${MOC_FILES})
target_link_libraries(kmines
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kmines RUNTIME DESTINATION ${KDE1_BINDIR})
