#add_subdirectory(doc)
add_subdirectory(pics)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KBLACKBOX_SRCS kbbgame.cpp kbbgfx.cpp main.cpp util.cpp)

set(KBLACKBOX_MOC kbbgame.moc kbbgfx.moc)

QT1_WRAP_MOC(MOC_FILES ${KBLACKBOX_MOC})

add_executable(kblackbox ${KBLACKBOX_SRCS} ${MOC_FILES})
target_link_libraries(kblackbox
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kblackbox RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kblackbox.kdelnk DESTINATION ${KDE1_DATADIR}/kpanel/default)
install_icon(kblackbox.xpm)
