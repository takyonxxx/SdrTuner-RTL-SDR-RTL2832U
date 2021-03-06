#-------------------------------------------------
#
# Project created by QtCreator 2017-06-24T12:46:07
#
#-------------------------------------------------
QT       += core gui network widgets

lessThan(QT_MAJOR_VERSION,5) {
    error("SdrTuner requires Qt 5.")
}

TARGET = SdrTuner
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    SdrWindow.cpp \
    freqctrl.cpp \
    plotter.cpp \
    meter.cpp \
    dsp/afsk1200/cafsk12.cpp \
    dsp/filter/decimator.cpp \
    dsp/filter/fir_decim.cpp \
    dsp/rds/decoder_impl.cc \
    dsp/rds/parser_impl.cc \
    dsp/agc_impl.cpp \
    dsp/correct_iq_cc.cpp \
    dsp/hbf_decim.cpp \
    dsp/lpf.cpp \
    dsp/resampler_xx.cpp \
    dsp/rx_agc_xx.cpp \
    dsp/rx_demod_am.cpp \
    dsp/rx_demod_fm.cpp \
    dsp/rx_fft.cpp \
    dsp/rx_filter.cpp \
    dsp/rx_meter.cpp \
    dsp/rx_noise_blanker_cc.cpp \
    dsp/rx_rds.cpp \
    dsp/sniffer_f.cpp \
    dsp/stereo_demod.cpp \
    receivers/nbrx.cpp \
    receivers/receiver_base.cpp \
    receivers/wfmrx.cpp \
    dsp/afsk1200/costabf.c \
    receiver.cpp \
    udp_sink_f.cpp \
    file_resources.cpp

HEADERS += \
    SdrWindow.h \
    freqctrl.h \
    plotter.h \
    meter.h \
    fmdemod.h \
    dsp/afsk1200/cafsk12.h \
    dsp/afsk1200/filter-i386.h \
    dsp/afsk1200/filter.h \
    dsp/filter/decimator.h \
    dsp/filter/filtercoef_hbf_100.h \
    dsp/filter/filtercoef_hbf_140.h \
    dsp/filter/filtercoef_hbf_70.h \
    dsp/filter/fir_decim.h \
    dsp/filter/fir_decim_coef.h \
    dsp/rds/api.h \
    dsp/rds/constants.h \
    dsp/rds/decoder.h \
    dsp/rds/decoder_impl.h \
    dsp/rds/parser.h \
    dsp/rds/parser_impl.h \
    dsp/rds/tmc_events.h \
    dsp/agc_impl.h \
    dsp/correct_iq_cc.h \
    dsp/hbf_decim.h \
    dsp/lpf.h \
    dsp/resampler_xx.h \
    dsp/rx_agc_xx.h \
    dsp/rx_demod_am.h \
    dsp/rx_demod_fm.h \
    dsp/rx_fft.h \
    dsp/rx_filter.h \
    dsp/rx_meter.h \
    dsp/rx_noise_blanker_cc.h \
    dsp/rx_rds.h \
    dsp/sniffer_f.h \
    dsp/stereo_demod.h \
    receivers/nbrx.h \
    receivers/receiver_base.h \
    receivers/wfmrx.h \
    receiver.h \
    udp_sink_f.h

FORMS += \
    sdrwindow.ui

PKGCONFIG_EXISTS = $$system(pkg-config --version)
isEmpty(PKGCONFIG_EXISTS) {
    message("pkg-config not found!")
}
else
{
    message("enable pkg-config to find dependencies")
    CONFIG += link_pkgconfig
}

win32{
message("win32 enabled")
LIBS += -L$$(BOOST_DIR)\lib64-msvc-12.0 \
        -lboost_system
        -lboost_thread
        -lboost_program


LIBS += -LC:/GNURadio-3.7/lib \
         -lgnuradio-analog
         -lgnuradio-blocks
         -lgnuradio-digital
         -lgnuradio-filter
         -lgnuradio-fft
         -lgnuradio-runtime
         -lgnuradio-audio
         -lgnuradio-osmosdr

INCLUDEPATH += $$(BOOST_DIR)
INCLUDEPATH += C:/GNURadio-3.7/include
}

macx{
message("macx enabled")

    INCLUDEPATH += /usr/local/lib
    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/local/Cellar/boost/1.70.0/include
    INCLUDEPATH += /usr/local/Cellar/gr-osmosdr/0.1.4_8/include

    LIBS += -L/usr/local/lib \
    -lboost_system
    -lboost_program_options
    -lboost_thread

    LIBS += -L/usr/local/Cellar/gr-osmosdr/0.1.4_8/lib -lgnuradio-osmosdr

    PKGCONFIG += gnuradio-analog \
                 gnuradio-blocks \
                 gnuradio-digital \
                 gnuradio-filter \
                 gnuradio-fft \
                 gnuradio-runtime \
                 gnuradio-audio

}

unix:!macx{
message("unix enabled")

    LIBPATH += /usr/lib
    LIBPATH += /usr/local/lib
    LIBPATH += /usr/lib/x86_64-linux-gnu

    LIBS += -lboost_system -lboost_program_options
    LIBS += -lrt  # need to include on some distros
    LIBS += -lasound

    PKGCONFIG += gnuradio-analog \
                 gnuradio-blocks \
                 gnuradio-digital \
                 gnuradio-filter \
                 gnuradio-fft \
                 gnuradio-runtime \
                 gnuradio-audio \
                 gnuradio-osmosdr

    INCLUDEPATH += /usr/local/include
}
