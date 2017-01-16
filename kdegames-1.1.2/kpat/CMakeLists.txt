#add_subdirectory(icons)
#add_subdirectory(pics)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KPAT_SRCS main.cpp patience.cpp rnd.cpp cardmaps.cpp
    computation.cpp klondike.cpp grandf.cpp ten.cpp
    basiccard.cpp card.cpp napoleon.cpp idiot.cpp
    microsol.cpp pwidget.cpp freecell.cpp mod3.cpp)

set(KPAT_MOC patience.moc computation.moc klondike.moc
    grandf.moc basiccard.moc card.moc mod3.moc
    ten.moc napoleon.moc idiot.moc freecell.moc
    microsol.moc pwidget.moc cardmaps.moc)

QT1_WRAP_MOC(MOC_FILES ${KPAT_MOC})

add_executable(kpat ${KPAT_SRCS} ${MOC_FILES})
target_link_libraries(kpat
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kpat RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kpat.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(kpat.xpm)
