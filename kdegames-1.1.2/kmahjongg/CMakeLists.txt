include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KMAHJONGG_SRCS kmahjongg.cpp static.cpp)

set(KMAHJONGG_MOC kmahjongg.moc)

QT1_WRAP_MOC(MOC_FILES ${KMAHJONGG_MOC})

add_executable(kmahjongg ${KMAHJONGG_SRCS} ${MOC_FILES})
target_link_libraries(kmahjongg
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kmahjongg RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kmahjongg.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(kmahjongg.xpm)
install_icon(kmahjongg.bmp "${KDE1_DATADIR}/kmahjongg/pics")
install_icon(kmahjongg_bgnd.bmp "${KDE1_DATADIR}/kmahjongg/pics")

install(FILES
    index.html index-1.html index-2.html logotp3.gif
    DESTINATION ${KDE1_HTMLDIR}/en/kmahjongg )
