include_directories( . ${RTL_SDR_INCLUDE_DIRECTORY}  ${LIBUSB_1_INCLUDE_DIRS} )

add_executable(spectrum main.cpp graph_container.cpp graph_widget.cpp rtldev.cpp scanner.cpp scanner_thread.cpp)
target_link_libraries (spectrum ${FLTK_LIBRARY} ${RTL_SDR_LIBS} ${LIBUSB_1_LIBRARIES} )

