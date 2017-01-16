add_subdirectory(html)
add_subdirectory(sprites)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KASTEROIDS_SRCS  kfixedtopwidget.cpp main.cpp toplevel.cpp view.cpp)

set(KASTEROIDS_MOC kfixedtopwidget.moc)

QT1_WRAP_MOC(MOC_FILES ${KASTEROIDS_MOC})

add_executable(kasteroids ${KASTEROIDS_SRCS} ${MOC_FILES})
target_link_libraries(kasteroids
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kasteroids RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kasteroids.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(kasteroids.xpm)

install(FILES bg.ppm DESTINATION ${KDE1_DATADIR}/kasteroids/sprites)
