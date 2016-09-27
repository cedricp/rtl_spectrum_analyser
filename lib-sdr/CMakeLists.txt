include_directories( .  ${LIBUSB_1_INCLUDE_DIRS} )

add_library(rtl librtlsdr.c  tuner_e4k.c  tuner_fc0012.c  tuner_fc0013.c  tuner_fc2580.c  tuner_r82xx.c)
