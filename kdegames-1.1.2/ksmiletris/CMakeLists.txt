add_subdirectory(data)
add_subdirectory(wav)

include_directories(
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KSMILETRIS_SRCS
    main.cpp gamewindow.cpp gamewidget.cpp screenwidget.cpp mirrorwidget.cpp
    npiecewidget.cpp scoredialog.cpp newscoredialog.cpp
)

set(KSMILETRIS_MOC
    gamewindow.moc gamewidget.moc screenwidget.moc mirrorwidget.moc
    npiecewidget.moc scoredialog.moc newscoredialog.moc
)

QT1_WRAP_MOC(MOC_FILES ${KSMILETRIS_MOC})

add_executable(ksmiletris ${KSMILETRIS_SRCS} ${MOC_FILES})
target_link_libraries(ksmiletris
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_MEDIATOOL}
)

install(TARGETS ksmiletris RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES ksmiletris.kdelnk DESTINATION ${KDE1_DATADIR}/ksmiletris)

install_icon(ksmiletris.xpm "${KDE1_DATADIR}/ksmiletris")
install(FILES mini-ksmiletris.xpm DESTINATION ${KDE1_MINIDIR} RENAME ksmiletris.xpm)
install(FILES large-ksmiletris.xpm DESTINATION ${KDE1_ICONDIR}/large RENAME ksmiletris.xpm)
