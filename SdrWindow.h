#ifndef SdrWindow_H
#define SdrWindow_H

#include <QMainWindow>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include "freqctrl.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <osmosdr/device.h>
#include <osmosdr/source.h>
#include <osmosdr/ranges.h>
#ifndef Q_MOC_RUN
#include "receiver.h"
#endif
#if defined(Q_OS_LINUX)
#include <alsa/asoundlib.h>
#endif

#define MHZ(x)                      ((x)*1000*1000)
#define KHZ(x)                      ((x)*1*1000)
#define DEFAULT_SAMPLE_RATE         MHZ(2.8)
#define DEFAULT_FREQUENCY		    MHZ(99.3)
#define DEFAULT_FFT_SIZE		    16384 * 2
#define DEFAULT_FFT_RATE		    120
#define DATA_BUFFER_SIZE            48000
#define SAMPLE_BUFFER_SIZE          1024
#define DEFAULT_FREQ_STEP           5 //kHz
#define DEFAULT_AUDIO_GAIN          152
#define DEFAULT_DEMOD_MODE          5

#define FILTER_PRESET_WIDE      0
#define FILTER_PRESET_NORMAL    1
#define FILTER_PRESET_NARROW    2
#define FILTER_PRESET_USER      3

namespace Ui {
class SdrWindow;
}

class SdrWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SdrWindow(QWidget *parent = 0);
    ~SdrWindow();

    static SdrWindow *instance();   
    void setNewFrequency(qint64 rx_freq);
    void getStatusFromConfigurationFile();
    void updateConfigurationFileFromStatus();
    void createConfigurationFile();
    void initConf();
    void updateConf();   

    enum rxopt_mode_idx {
        MODE_OFF        = 0, /*!< Demodulator completely off. */
        MODE_RAW        = 1, /*!< Raw I/Q passthrough. */
        MODE_AM         = 2, /*!< Amplitude modulation. */
        MODE_NFM        = 3, /*!< Narrow band FM. */
        MODE_WFM_MONO   = 4, /*!< Broadcast FM (mono). */
        MODE_WFM_STEREO = 5, /*!< Broadcast FM (stereo). */
        MODE_LSB        = 6, /*!< Lower side band. */
        MODE_USB        = 7, /*!< Upper side band. */
        MODE_CWL        = 8, /*!< CW using LSB filter. */
        MODE_CWU        = 9, /*!< CW using USB filter. */
        MODE_WFM_STEREO_OIRT = 10, /*!< Broadcast FM (stereo oirt). */
        MODE_LAST       = 11
    };

    // Filter preset table per mode, preset and lo/hi
    const int filter_preset_table[MODE_LAST][3][2] =
    {   //     WIDE             NORMAL            NARROW
        {{      0,      0}, {     0,     0}, {     0,     0}},  // MODE_OFF
        {{ -15000,  15000}, { -5000,  5000}, { -1000,  1000}},  // MODE_RAW
        {{ -10000,  10000}, { -5000,  5000}, { -2500,  2500}},  // MODE_AM
        {{ -10000,  10000}, { -5000,  5000}, { -2500,  2500}},  // MODE_NFM
        {{-100000, 100000}, {-80000, 80000}, {-60000, 60000}},  // MODE_WFM_MONO
        {{-100000, 100000}, {-80000, 80000}, {-60000, 60000}},  // MODE_WFM_STEREO
        {{  -4000,   -100}, { -2800,  -100}, { -2400,  -300}},  // MODE_LSB
        {{    100,   4000}, {   100,  2800}, {   300,  2400}},  // MODE_USB
        {{  -1000,   1000}, {  -250,   250}, {  -100,   100}},  // MODE_CWL
        {{  -1000,   1000}, {  -250,   250}, {  -100,   100}},  // MODE_CWU
        {{-100000, 100000}, {-80000, 80000}, {-60000, 60000}}   // MODE_WFM_STEREO_OIRT
    };

    QStringList ModulationStrings;

private:

    char log_buffer[250];

    receiver *rx;
    std::map<QString, QVariant> devList;
    qint64 frequency;
    unsigned int sampleRate;
    unsigned int audioGain;
    unsigned int freqStep;
    unsigned int fftSize;
    unsigned int fftrate;
    unsigned int lastMode;


    qint64 d_hw_freq_start;
    qint64 d_hw_freq_stop;
    int         m_HiCutFreq;
    int         m_LowCutFreq;

    enum receiver::filter_shape d_filter_shape;
    QTimer   *dec_timer;
    QTimer   *iq_fft_timer;
    QTimer   *meter_timer;
    QTimer   *rds_timer;

    bool           udpEnabled;
    std::string    udpHost;     /*! UDP client host name. */
    int            udpPort;     /*! UDP client port number. */

    std::complex<float>* d_fftData;
    float          *d_realFftData;
    float          *d_iirFftData;
    float          *d_pwrFftData;
    //double *d_audioFttData;
    float           d_fftAvg;      /*!< FFT averaging parameter set by user (not the true gain). */

    void initObjects();
    bool startTuner(std::map<QString, QVariant> &devList);
    void AppentTextBrowser(const char* stringBuffer);
    void initSpectrumGraph();
    int  setFftSize(int fft_size);
    int  setFreqStep(int step);
    int  setFftRate(int rate);
    void updateInfoLabels(void);

    void getFilterPreset(int mode, int preset, int * lo, int * hi) const;    
    void selectDemod(int mode_idx);

    int  currentDemod() const;     
    void setFreqCorr(double ppm);
    void setGain(QString name, double gain);
    void setOutputDevice();
    unsigned int getOutDeviceCount( void );    


private slots:
    void on_push_connect_clicked();
    void on_push_exit_clicked();
    void on_plotter_newDemodFreq(qint64 freq, qint64 delta);   /*! New demod freq (aka. filter offset). */
    void on_fftSizeComboBox_currentIndexChanged(const QString &arg1);
    void on_audioGainSlider_valueChanged(int value);
    void iqFftTimeout();
    void meterTimeout();
    void on_modeSelector_currentIndexChanged(int index);
    void decoderTimeout(); 
    void startAudioStreaming(const std::string udp_host, int udp_port);
    void stopAudioStreaming();
    void on_plotter_newFilterFreq(int low, int high);    /*! New filter width */
    void on_freqStepSelector_currentIndexChanged(const QString &arg1);
    void on_fftRateSelector_currentIndexChanged(const QString &arg1);
    void on_udpEnabled_clicked(bool checked);
    void on_waterFallColor_currentIndexChanged(int index);

private:
    Ui::SdrWindow *ui;
    static SdrWindow *theInstance_;
};



#endif // SdrWindow_H
