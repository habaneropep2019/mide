add_subdirectory(pics)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KMOON_SRCS  kmoon.cpp phases.cpp)

set(KMOON_MOC kmoon.moc)

QT1_WRAP_MOC(MOC_FILES ${KMOON_MOC})

add_executable(kmoon ${KMOON_SRCS} ${MOC_FILES})
target_link_libraries(kmoon
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kmoon RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kmoon.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
install_icon(kmoon.xpm)
