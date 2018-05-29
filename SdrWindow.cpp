#include "SdrWindow.h"
#include "ui_sdrwindow.h"
SdrWindow *SdrWindow::theInstance_;
using namespace  std;

SdrWindow::SdrWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SdrWindow),
    frequency(DEFAULT_FREQUENCY),
    sampleRate(0),
    m_HiCutFreq(KHZ(5)),
    m_LowCutFreq(-KHZ(5))
{
    ui->setupUi(this);
    theInstance_ = this;

    ui->push_connect->setEnabled(false);
    sampleRate = DEFAULT_SAMPLE_RATE;
    frequency  = DEFAULT_FREQUENCY;
    fftSize    = DEFAULT_FFT_SIZE;
    fftrate    = DEFAULT_FFT_RATE;
    freqStep   = DEFAULT_FREQ_STEP;
    audioGain  = DEFAULT_AUDIO_GAIN;
    lastMode   = DEFAULT_DEMOD_MODE;
    //default values, you may change values from settings.ini
    udpEnabled = true;
    udpHost   = std::string( "127.0.0.1");
    udpPort   = 5000;

    d_fftData = new std::complex<float>[MAX_FFT_SIZE];
    d_realFftData = new float[MAX_FFT_SIZE];
    d_pwrFftData = new float[MAX_FFT_SIZE]();
    d_iirFftData = new float[MAX_FFT_SIZE];
    for (int i = 0; i < MAX_FFT_SIZE; i++)
        d_iirFftData[i] = -140.0;  // dBFS

    initConf();
    initObjects();
    initSpectrumGraph();

    rx = new receiver("", "", 1);
   // rx->set_rf_freq(frequency);

    /* timer for data decoders */
    dec_timer = new QTimer(this);
    connect(dec_timer, SIGNAL(timeout()), this, SLOT(decoderTimeout()));

    /* meter timer */
    meter_timer = new QTimer(this);
    connect(meter_timer, SIGNAL(timeout()), this, SLOT(meterTimeout()));

    /* FFT timer & data */
    iq_fft_timer = new QTimer(this);
    connect(iq_fft_timer, SIGNAL(timeout()), this, SLOT(iqFftTimeout()));

    if(startTuner(devList))
    {
        rx->set_input_rate(DEFAULT_SAMPLE_RATE);
        rx->set_input_decim(1);
        rx->set_analog_bandwidth(0);
        rx->set_auto_gain(true);
        rx->set_antenna("RX");
        //setGain(QString("LNA"),16640);
        setNewFrequency(frequency);
        ui->plotter->setSampleRate(sampleRate);
        ui->audioGainSlider->setValue(audioGain);
        ui->modeSelector->setCurrentIndex(lastMode);
        ui->push_connect->setEnabled(true);
        ui->udpEnabled->setChecked(udpEnabled);
        ui->waterFallColor->setCurrentIndex(COLPAL_BLUE);

        setFftSize(fftSize);
        setFreqStep(freqStep);
        setFftRate(fftrate);

        if (rx->start_sniffer(sampleRate, SAMPLE_BUFFER_SIZE) == receiver::STATUS_OK)
        {
            dec_timer->start(100);
        }
    }
}

SdrWindow *SdrWindow::instance()
{
    return theInstance_;
}

SdrWindow::~SdrWindow()
{
    dec_timer->stop();
    delete dec_timer;
    meter_timer->stop();
    delete meter_timer;
    iq_fft_timer->stop();
    delete iq_fft_timer;

    delete [] d_fftData;
    delete [] d_realFftData;
    delete [] d_iirFftData;
    delete [] d_pwrFftData;

    delete rx;
    delete ui;
}

int  SdrWindow::currentDemod() const
{
    return ui->modeSelector->currentIndex();
}

void SdrWindow::on_udpEnabled_clicked(bool checked)
{
    udpEnabled = checked;

    if(udpEnabled)
        startAudioStreaming(udpHost,udpPort);
    else
         stopAudioStreaming();

    updateConf();
}

void SdrWindow::on_push_connect_clicked()
{
    if(ui->push_connect->text() == "Connect")
    {
        rx->start();
        meter_timer->start(100);
        iq_fft_timer->start(1000/fftrate);
        ui->plotter->setFftRate(fftrate);
        if(udpEnabled)
        startAudioStreaming(udpHost,udpPort);

        ui->push_connect->setText("DisConnect");
    }
    else
    {
        if(udpEnabled)
        stopAudioStreaming();

        meter_timer->stop();
        iq_fft_timer->stop();
        rx->stop();
        ui->push_connect->setText("Connect");
    }
}

void SdrWindow::on_push_exit_clicked()
{
    updateConf();
    exit(0);
}

void SdrWindow::on_fftSizeComboBox_currentIndexChanged(const QString &arg1)
{
    int value = arg1.toInt();
    fftSize = value;
    rx->set_iq_fft_size(value);
    updateInfoLabels();
    updateConf();
}

void SdrWindow::on_fftRateSelector_currentIndexChanged(const QString &arg1)
{
    int value = arg1.toInt();
    fftrate = value;
    iq_fft_timer->stop();
    iq_fft_timer->start(1000/fftrate);
    ui->plotter->setFftRate(fftrate);
    updateConf();;
}

void SdrWindow::on_audioGainSlider_valueChanged(int value)
{
    audioGain = value;
    float gain = float(value) / 10.0;

    // update dB label
    ui->audioGainDbLabel->setText(QString("%1 dB").arg(gain, 5, 'f', 1));
    rx->set_af_gain(gain);
    updateConf();
}

void SdrWindow::on_modeSelector_currentIndexChanged(int index)
{
    lastMode = index;
    selectDemod(index);
    updateConf();
}

void SdrWindow::on_freqStepSelector_currentIndexChanged(const QString &arg1)
{
    freqStep = arg1.toInt();
    ui->plotter->setWheelConstant(KHZ(freqStep));
    updateConf();
}

/* CPlotter::NewDemodFreq() is emitted */
void SdrWindow::on_plotter_newDemodFreq(qint64 freq, qint64 delta)
{
    // set RX filter
    rx->set_filter_offset((double) delta);
    setNewFrequency(freq);
}

void SdrWindow::setNewFrequency(qint64 rx_freq)
{
    frequency = (qint64)rx_freq;
    // set receiver frequency

    double hw_freq = (double)(rx_freq) - rx->get_filter_offset();

    // set receiver frequency
    rx->set_rf_freq(hw_freq);

    ui->freqCtrl->setFrequency(rx_freq);
    ui->plotter->setCenterFreq(frequency);
    updateConf();

    snprintf(log_buffer, sizeof(log_buffer),
             "Set Frequency : %.2f Mhz",(float)hw_freq/1000000.0f);

    AppentTextBrowser(log_buffer);
}


/* CPlotter::NewfilterFreq() is emitted or bookmark activated */
void SdrWindow::on_plotter_newFilterFreq(int low, int high)
{
    receiver::status retcode;
    m_LowCutFreq = low;
    m_HiCutFreq = high;

    if(m_HiCutFreq > 120e3)
    {
        /* Broadcast FM */
        //ui->plotter->setDemodRanges(-120e3, -10000, 10000, 120e3, true);
    }

    /* parameter correctness will be checked in receiver class */
    retcode = rx->set_filter((double) m_LowCutFreq, (double) m_HiCutFreq, d_filter_shape);

    if (retcode == receiver::STATUS_OK)
    {
        ui->plotter->setHiLowCutFrequencies(m_LowCutFreq, m_HiCutFreq);
        ui->filterFreq->setFrequency(m_HiCutFreq);
        snprintf(log_buffer, sizeof(log_buffer),
                 "Set Filter-> low: %.2f Khz - high: %.2f Khz",(float)m_LowCutFreq/1000.0f,(float)m_HiCutFreq/1000.0f);

        AppentTextBrowser(log_buffer);
    }
}

/**
 * Cyclic processing for acquiring samples from receiver and processing them
 * with data decoders (see dec_* objects)
 */
void SdrWindow::decoderTimeout()
{
    float buffer[SAMPLE_BUFFER_SIZE];
    unsigned int num;

    rx->get_sniffer_data(&buffer[0], num);
   // printf("buffer: %f\n",buffer[0]);
}

/** Start streaming audio over UDP. */
void SdrWindow::startAudioStreaming(const std::string udp_host, int udp_port)
{
    rx->start_udp_streaming(udp_host, udp_port);
}

/** Stop streaming audio over UDP. */
void SdrWindow::stopAudioStreaming()
{
    rx->stop_udp_streaming();
}

///OBJECT FUNCTIONS

void SdrWindow::initObjects()
{
    QString const& pingURL = QString("xset -dpms");
    int status = system(pingURL.toStdString().c_str());
    if (-1 != status)
    {
        printf("Monitor auto turn off, Disabled.\n");
    }
    ui->push_exit->setStyleSheet("color: white;background-color: CadetBlue;");
    ui->push_connect->setStyleSheet("color: white;background-color: CadetBlue;");
    ui->text_terminal->setStyleSheet("font: 12px; color: #00cccc; background-color: #001a1a;");

    ui->filterFreq->setup(0, 0 ,KHZ(1000), 1,FCTL_UNIT_KHZ);
    ui->filterFreq->setDigitColor(QColor("#ff9933"));
    ui->filterFreq->setFrequency(0);

    ui->freqCtrl->setup(0, 0, 9999e6, 1, FCTL_UNIT_MHZ);
    ui->freqCtrl->setDigitColor(QColor("#ff9933"));
    ui->freqCtrl->setFrequency(frequency);
}


void SdrWindow::initSpectrumGraph()
{
    /* set up FFT */

    ui->plotter->setTooltipsEnabled(true);

    ui->plotter->setSampleRate(sampleRate);
    ui->plotter->setSpanFreq((quint32)sampleRate);
    ui->plotter->setCenterFreq(frequency);

    ui->plotter->setFftRange(-140.0f, 20.0f);
    ui->plotter->setPandapterRange(-140.f, 20.f);
   // ui->plotter->setHiLowCutFrequencies(m_LowCutFreq,m_HiCutFreq);
   // ui->plotter->setDemodRanges(m_LowCutFreq, -KHZ(5), KHZ(5),m_HiCutFreq, true);

    ui->plotter->setFreqUnits(1000);
    ui->plotter->setPercent2DScreen(50);
    ui->plotter->setFilterBoxEnabled(true);
    ui->plotter->setCenterLineEnabled(true);
    ui->plotter->setClickResolution(10);

    ui->plotter->setFftPlotColor(QColor("#CEECF5"));
    ui->plotter->setWheelConstant(KHZ(25));

    //ui->plotter->setPeakDetection(true ,2);
    ui->plotter->setFftFill(true);
}

void SdrWindow::AppentTextBrowser(const char* stringBuffer)
{
    ui->text_terminal->append(QString::fromUtf8(stringBuffer));
    ui->text_terminal->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
}

///SETTINGS.INI FUNCTIONS
///
void SdrWindow::initConf()
{    try
    {
        getStatusFromConfigurationFile();
    }
    catch(std::exception const& ex)
    {
        printf("File settings.ini not found.\nCreating settings.ini file\n");
        createConfigurationFile();
        getStatusFromConfigurationFile();
    }
}

void SdrWindow::updateConf()
{
    try
    {
        updateConfigurationFileFromStatus();
    }
    catch(std::exception const& ex)
    {
        printf("Exception handled: %s.", ex.what());
    }
}

void SdrWindow::getStatusFromConfigurationFile() {
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("settings.ini", pt);

    frequency  = pt.get<int>("Radio.frequency");
    sampleRate = pt.get<int>("Radio.sampleRate");
    fftSize    = pt.get<int>("Radio.fftSize");
    fftrate    = pt.get<int>("Radio.fftrate");
    freqStep   = pt.get<int>("Radio.freqStep");
    audioGain  = pt.get<int>("Radio.audioGain");
    lastMode   = pt.get<int>("Radio.lastMode");
    udpEnabled = pt.get<bool>("Radio.udpEnabled");
    udpHost    = pt.get<std::string>("Radio.udpHost");
    udpPort    = pt.get<int>("Radio.udpPort");
}

void SdrWindow::updateConfigurationFileFromStatus() {
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("settings.ini", pt);

    pt.put("Radio.frequency", frequency);
    pt.put("Radio.sampleRate", sampleRate);
    pt.put("Radio.fftSize", fftSize);
    pt.put("Radio.fftrate", fftrate);
    pt.put("Radio.freqStep", freqStep);
    pt.put("Radio.audioGain", audioGain);
    pt.put("Radio.lastMode", lastMode);
    pt.put("Radio.udpEnabled", udpEnabled);
    pt.put("Radio.udpHost", udpHost);
    pt.put("Radio.udpPort", udpPort);

    boost::property_tree::ini_parser::write_ini("settings.ini", pt);
}

void SdrWindow::createConfigurationFile() {
    boost::property_tree::ptree pt;

    //create with default parameters
    pt.put("Radio.frequency", frequency);
    pt.put("Radio.sampleRate", sampleRate);
    pt.put("Radio.fftSize", fftSize);
    pt.put("Radio.fftrate", fftrate);
    pt.put("Radio.freqStep", freqStep);
    pt.put("Radio.audioGain", audioGain);
    pt.put("Radio.lastMode", lastMode);
    pt.put("Radio.udpEnabled", udpEnabled);
    pt.put("Radio.udpHost", udpHost);
    pt.put("Radio.udpPort", udpPort);
    boost::property_tree::ini_parser::write_ini("settings.ini", pt);
}

///RADIO TUNER FUNCTIONS

bool SdrWindow::startTuner(std::map<QString, QVariant> &devList)
{

    unsigned int i=0;
    QString devstr;
    QString devlabel;

    d_hw_freq_start = (quint64) 0;
    d_hw_freq_stop  = (quint64) 9999e6;

    bool deviceFound = false;

    // Get list of input devices discovered by gr-osmosdr and store them in
    // the device list together with the device descriptor strings
    osmosdr::devices_t devs = osmosdr::device::find();

    BOOST_FOREACH(osmosdr::device_t &dev, devs)
    {
        if (dev.count("label"))
        {
            devlabel = QString(dev["label"].c_str());
            dev.erase("label");
        }
        else
        {
            devlabel = "Unknown";
        }

        devstr = QString(dev.to_string().c_str());
        devList.insert(std::pair<QString, QVariant>(devlabel, devstr));

        if(devlabel.contains("RTL2838"))
        {
            AppentTextBrowser(QString("Found Tuner : %1\n%2").arg(devstr).arg(devlabel).toStdString().c_str());
            setWindowTitle(QString("SdrTuner - %2").arg(devlabel));
            rx->set_input_device(devstr.toStdString());

            setOutputDevice();

            deviceFound = true;
        }
        ++i;
    }

    if(!deviceFound)
    {
        AppentTextBrowser("No device found...");
    }

    return deviceFound;
}

void SdrWindow::setOutputDevice()
{
  void **hints;
  const char *ifaces[] = {"pcm", 0};
  int index = 0;
  void **str;
  char *name;
  char *desc;
  int devIdx = 0;
  size_t tPos,tPos1;

  snd_config_update();

  while (ifaces[index]) {

    printf("\Querying interface %s \n", ifaces[index]);
    if (snd_device_name_hint(-1, ifaces[index], &hints) < 0)
    {
      printf("Querying devices failed for %s.\n", ifaces[index]);
      index++;
      continue;
    }
    str = hints;
    while (*str)
    {
      name = snd_device_name_get_hint(*str, "NAME");
      desc = snd_device_name_get_hint(*str, "DESC");

      string tNameStr = "";
      if (name != NULL)
          tNameStr = string(name);

      // search for "default:", if negative result then go on searching for next device
      if ((tNameStr != "") && ((tPos = tNameStr.find("default:")) != string::npos) && ((tPos1 = tNameStr.find("CARD=")) != string::npos))
      {
          printf("Deafult Sound Card : %d : %s\n%s\n",devIdx, name,desc);
          QString outdev = name;
          rx->set_output_device(outdev.toStdString());
          snd_device_name_free_hint(hints);

          return;
      }

      free(name);
      free(desc);
      devIdx++;
      str++;
    }
    index++;
    snd_device_name_free_hint(hints);
  }
  return;
}

void SdrWindow::setGain(QString name, double gain)
{
    rx->set_gain(name.toStdString(), gain);
}

void SdrWindow::setFreqCorr(double ppm)
{
    if (ppm < -200.0)
        ppm = -200.0;
    else if (ppm > 200.0)
        ppm = 200.0;

    qDebug() << __FUNCTION__ << ":" << ppm << "ppm";
    rx->set_freq_corr(ppm);
}

void SdrWindow::selectDemod(int mode_idx)
{
    double  cwofs = 0.0;
    int     filter_preset = 1;
    int     flo=0, fhi=0, click_res=100;

    qDebug() << "New mode index:" << mode_idx;

    getFilterPreset(mode_idx, filter_preset, &flo, &fhi);
    d_filter_shape = receiver::FILTER_SHAPE_NORMAL;
    m_LowCutFreq = flo;
    m_HiCutFreq = fhi;
    switch (mode_idx) {

    case MODE_OFF:
        rx->set_demod(receiver::RX_DEMOD_OFF);
        click_res = 1000;
        break;

    case MODE_RAW:
        /* Raw I/Q; max 96 ksps*/
        rx->set_demod(receiver::RX_DEMOD_NONE);
        ui->plotter->setDemodRanges(-40000, -200, 200, 40000, true);
        click_res = 100;
        break;

    case MODE_AM:
        rx->set_demod(receiver::RX_DEMOD_AM);
        ui->plotter->setDemodRanges(-40000, -200, 200, 40000, true);
        click_res = 100;
        break;

    case MODE_NFM:
        ui->plotter->setDemodRanges(-40000, -1000, 1000, 40000, true);
        rx->set_demod(receiver::RX_DEMOD_NFM);
        click_res = 100;
        break;

    case MODE_WFM_MONO:
        rx->set_demod(receiver::RX_DEMOD_WFM_M);
        click_res = 100;
        break;
    case MODE_WFM_STEREO:
        rx->set_demod(receiver::RX_DEMOD_WFM_S);
        click_res = 100;
        break;
       break;
    case MODE_WFM_STEREO_OIRT:
        /* Broadcast FM */
        ui->plotter->setDemodRanges(-120e3, -10000, 10000, 120e3, true);
        click_res = 100;
        if (mode_idx == MODE_WFM_MONO)
            rx->set_demod(receiver::RX_DEMOD_WFM_M);
        else if (mode_idx == MODE_WFM_STEREO_OIRT)
            rx->set_demod(receiver::RX_DEMOD_WFM_S_OIRT);
        else
            rx->set_demod(receiver::RX_DEMOD_WFM_S);
        break;

    case MODE_LSB:
        /* LSB */
        rx->set_demod(receiver::RX_DEMOD_SSB);
        ui->plotter->setDemodRanges(-40000, -100, -5000, 0, false);
        click_res = 100;
        break;

    case MODE_USB:
        /* USB */
        rx->set_demod(receiver::RX_DEMOD_SSB);
        ui->plotter->setDemodRanges(0, 5000, 100, 40000, false);
        click_res = 100;
        break;

    case MODE_CWL:
        /* CW-L */
        rx->set_demod(receiver::RX_DEMOD_SSB);
        ui->plotter->setDemodRanges(-5000, -100, 100, 5000, true);
        click_res = 10;
        break;

    case MODE_CWU:
        /* CW-U */
        rx->set_demod(receiver::RX_DEMOD_SSB);
        ui->plotter->setDemodRanges(-5000, -100, 100, 5000, true);
        click_res = 10;
        break;

    default:
        qDebug() << "Unsupported mode selection (can't happen!): " << mode_idx;
        flo = -5000;
        fhi = 5000;
        click_res = 100;
        break;
    }

    qDebug() << "Filter preset for mode" << mode_idx << "LO:" << flo << "HI:" << fhi;

    rx->set_filter((double)flo, (double)fhi, d_filter_shape);
    rx->set_cw_offset(cwofs);

    ui->plotter->setSampleRate(sampleRate);
    ui->plotter->setSpanFreq((quint32)sampleRate);
    ui->plotter->setCenterFreq(frequency);

    ui->plotter->setHiLowCutFrequencies(m_LowCutFreq, m_HiCutFreq);
    ui->filterFreq->setFrequency(m_HiCutFreq);

    ui->plotter->setClickResolution(click_res);
    ui->plotter->setFilterClickResolution(click_res);
}

/** Get filter lo/hi for a given mode and preset */
void SdrWindow::getFilterPreset(int mode, int preset, int * lo, int * hi) const
{
    if (mode < 0 || mode >= MODE_LAST)
    {
        qDebug() << __func__ << ": Invalid mode:" << mode;
        mode = MODE_AM;
    }
    else if (preset < 0 || preset > 2)
    {
        qDebug() << __func__ << ": Invalid preset:" << preset;
        preset = FILTER_PRESET_NORMAL;
    }
    *lo = filter_preset_table[mode][preset][0];
    *hi = filter_preset_table[mode][preset][1];
}

int SdrWindow::setFftSize(int fft_size)
{
    int idx = -1;
    QString size_str = QString::number(fft_size);

    idx = ui->fftSizeComboBox->findText(size_str, Qt::MatchExactly);
    if(idx != -1)
        ui->fftSizeComboBox->setCurrentIndex(idx);

    updateInfoLabels();

    return 0;
}

int SdrWindow::setFreqStep(int step)
{
    int idx = -1;
    QString size_str = QString::number(step);

    idx = ui->freqStepSelector->findText(size_str, Qt::MatchExactly);
    if(idx != -1)
        ui->freqStepSelector->setCurrentIndex(idx);

    return 0;
}

int SdrWindow::setFftRate(int rate)
{
    int idx = -1;
    QString size_str = QString::number(rate);

    idx = ui->fftRateSelector->findText(size_str, Qt::MatchExactly);
    if(idx != -1)
        ui->fftRateSelector->setCurrentIndex(idx);

    return 0;
}

void SdrWindow::updateInfoLabels(void)
{
    float   size;
    float   rbw;

    if (sampleRate == 0.f)
        return;

    size = fftSize;
    if(size<0)
        return;

    rbw = sampleRate / size;
    if (rbw < 1.e3f)
        ui->fftRbwLabel->setText(QString("RBW: %1 Hz").arg(rbw, 0, 'f', 1));
    else if (rbw < 1.e6f)
        ui->fftRbwLabel->setText(QString("RBW: %1 kHz").arg(1.e-3 * rbw, 0, 'f', 1));
    else
        ui->fftRbwLabel->setText(QString("RBW: %1 MHz").arg(1.e-6 * rbw, 0, 'f', 1));
}

/** Signal strength meter timeout. */
void SdrWindow::meterTimeout()
{
    float level;
    level = rx->get_signal_pwr(true);
    ui->sMeter->setLevel(level);
}

/** Baseband FFT plot timeout. */
void SdrWindow::iqFftTimeout()
{
    unsigned int    fftsize;
    unsigned int    i;
    float           pwr;
    float           pwr_scale;
    float           lpwr;
    float           gain;
    std::complex<float> pt;     /* a single FFT point used in calculations */

    //75 is default
    d_fftAvg = 1.0 - 1.0e-2 * ((float)90);

    // FIXME: fftsize is a reference
    rx->get_iq_fft_data(d_fftData, fftsize);

    if (fftsize == 0)
    {
        /* nothing to do, wait until next activation. */
        return;
    }

    // NB: without cast to float the multiplication will overflow at 64k
    // and pwr_scale will be inf
    pwr_scale = 1.0 / ((float)fftsize * (float)fftsize);

    /* Normalize, calculate power and shift the FFT */
    for (i = 0; i < fftsize; i++)
    {

        /* normalize and shift */
        if (i < fftsize/2)
        {
            pt = d_fftData[fftsize/2+i];
        }
        else
        {
            pt = d_fftData[i-fftsize/2];
        }

        /* calculate power in dBFS */
        pwr = pwr_scale * (pt.imag() * pt.imag() + pt.real() * pt.real());

        lpwr = 10.f * log10(pwr + 1.0e-20f);
        gain = 0.3 * (100.f + lpwr) / 100.f;
        d_realFftData[i] = d_realFftData[i] * (1.f - gain) + lpwr * gain;

        //d_realFftData[i] = 10.0 * log10f(pwr + 1.0e-20);
        /* FFT averaging */
        d_iirFftData[i] += d_fftAvg * (d_realFftData[i] - d_iirFftData[i]);
    }

    ui->plotter->setNewFttData(d_iirFftData, d_realFftData, fftsize);
}

void SdrWindow::on_waterFallColor_currentIndexChanged(int index)
{
    ui->plotter->setWaterfallPalette(index);
    updateConf();
}
