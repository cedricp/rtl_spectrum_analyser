include_directories( . ../lib-sdr ${LIBUSB_1_INCLUDE_DIRS} ${FLTK_INCLUDE_DIRECTORY} ${OPENGL_INCLUDE_DIR} )

add_executable(spectrum main.cpp graph_container.cpp graph_widget.cpp rtldev.cpp scanner.cpp scanner_thread.cpp)
target_link_libraries (spectrum ${FLTK_LIBS} rtl ${LIBUSB_1_LIBRARIES} ${OPENGL_LIBRARIES} ${X11_LIBRARIES} ${X11_Xft_LIB} ${X11_Xcursor_LIB} ${X11_Xinerama_LIB})

