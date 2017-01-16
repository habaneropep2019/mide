add_subdirectory(pics)

include_directories (
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
)

set(KWORLDWATCH_SRCS  astro.c kinputline.cpp main.cpp sunclock.c toplevel.cpp world.cpp)

set(KWORLDWATCH_MOC kinputline.moc toplevel.moc world.moc)

QT1_WRAP_MOC(MOC_FILES ${KWORLDWATCH_MOC})

add_executable(kworldwatch ${KWORLDWATCH_SRCS} ${MOC_FILES})
target_link_libraries(kworldwatch
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kworldwatch RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES KWorldWatch.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
install_icon(kworldwatch.xpm)
