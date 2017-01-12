add_subdirectory(docs)
add_subdirectory(pics)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KONQUEST_SRCS Konquest.cc gameboard.cc gamecore.cc int_validator.cc
    mainwin.cc map_widget.cc minimap.cc newgamedlg.cc planet_info.cc
    gameenddlg.cc  scoredlg.cc)

set(KONQUEST_MOC gameboard.moc gamecore.moc int_validator.moc mainwin.moc
    map_widget.moc minimap.moc newgamedlg.moc planet_info.moc scoredlg.moc
    gameenddlg.moc)

QT1_WRAP_MOC(MOC_FILES ${KONQUEST_MOC})

add_executable(konquest ${KONQUEST_SRCS} ${MOC_FILES})
target_link_libraries(konquest
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS konquest RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES Konquest.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(konquest.xpm)
