#ifndef PLOTTER_H
#define PLOTTER_H

#include <QtGui>
#include <QFont>
#include <QFrame>
#include <QImage>
#include <vector>
#include <QMap>
#include "SdrWindow.h"

#define HORZ_DIVS_MAX 12    //50
#define VERT_DIVS_MIN 5
#define MAX_SCREENSIZE 16384

#define PEAK_CLICK_MAX_H_DISTANCE 10 //Maximum horizontal distance of clicked point from peak
#define PEAK_CLICK_MAX_V_DISTANCE 20 //Maximum vertical distance of clicked point from peak
#define PEAK_H_TOLERANCE 2
#define COLPAL_DEFAULT  0
#define COLPAL_GRAY     1
#define COLPAL_BLUE     2

class CPlotter : public QFrame
{
    Q_OBJECT

public:
    explicit CPlotter(QWidget *parent = 0);
    ~CPlotter();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    const quint8 PALTBL[256*3] =
    {
        252,238,251,
        252,230,251,
        252,218,251,
        251,208,254,
        249,201,253,
        250,195,253,
        255,182,253,
        250,172,248,
        250,162,249,
        250,155,249,
        253,146,252,
        255,133,252,
        255,124,252,
        251,115,249,
        252,109,251,
        255,97,255,
        249,87,251,
        251,79,253,
        251,69,252,
        251,62,252,
        255,54,254,
        252,44,252,
        250,37,251,
        252,25,252,
        255,21,255,
        254,15,255,
        252,7,252,
        252,2,250,
        253,0,248,
        254,1,242,
        253,1,234,
        251,2,228,
        252,0,221,
        254,0,220,
        253,0,215,
        253,1,208,
        253,1,200,
        254,0,192,
        254,1,188,
        254,0,182,
        253,1,176,
        251,0,167,
        251,1,163,
        250,0,160,
        251,1,153,
        250,1,145,
        251,1,137,
        252,1,134,
        251,1,127,
        250,1,121,
        252,1,117,
        253,1,114,
        252,0,111,
        252,1,106,
        252,1,98,
        251,1,88,
        252,1,80,
        252,1,78,
        252,0,73,
        253,0,67,
        253,1,62,
        253,0,57,
        253,0,54,
        252,1,44,
        252,1,34,
        253,1,26,
        252,0,21,
        250,0,14,
        251,0,5,
        250,2,2,
        251,8,2,
        252,14,2,
        252,26,2,
        251,37,0,
        252,48,0,
        254,57,2,
        255,63,1,
        252,72,0,
        252,79,0,
        251,85,0,
        250,93,0,
        249,96,2,
        251,104,1,
        252,110,2,
        255,113,3,
        255,113,1,
        253,118,0,
        254,124,2,
        252,127,1,
        253,132,1,
        255,134,3,
        251,137,2,
        252,142,1,
        250,145,1,
        254,150,3,
        251,152,0,
        253,154,0,
        253,159,1,
        253,164,0,
        251,169,0,
        251,169,0,
        251,174,0,
        252,179,0,
        252,181,1,
        248,183,0,
        252,189,0,
        251,192,0,
        252,198,1,
        251,203,0,
        252,205,1,
        251,209,0,
        248,211,0,
        251,216,0,
        252,217,1,
        254,220,1,
        254,224,2,
        254,229,2,
        253,235,1,
        251,238,0,
        252,239,0,
        253,244,1,
        251,247,0,
        250,251,0,
        247,252,2,
        242,251,2,
        232,251,1,
        223,252,2,
        214,250,2,
        210,250,3,
        205,251,3,
        197,249,0,
        187,252,0,
        182,253,1,
        177,252,1,
        168,251,0,
        159,251,0,
        150,252,2,
        146,252,2,
        140,250,1,
        132,251,1,
        121,252,0,
        113,252,1,
        104,253,3,
        72,251,0,
        39,253,0,
        13,252,0,
        2,251,1,
        1,249,3,
        0,243,1,
        1,238,0,
        1,235,0,
        3,233,3,
        1,231,1,
        1,226,2,
        2,221,3,
        0,217,4,
        1,215,3,
        0,211,8,
        1,206,25,
        0,203,36,
        1,200,49,
        1,197,53,
        0,192,53,
        0,189,61,
        1,185,73,
        0,180,83,
        1,178,88,
        2,173,95,
        1,171,98,
        1,166,106,
        1,163,116,
        0,158,118,
        2,156,130,
        2,152,140,
        1,148,142,
        1,144,148,
        3,139,161,
        2,138,164,
        1,133,171,
        1,128,179,
        2,127,183,
        2,123,192,
        2,118,201,
        0,114,202,
        1,109,210,
        2,107,214,
        2,106,217,
        1,102,220,
        0,97,222,
        0,94,228,
        0,92,229,
        0,89,233,
        0,86,235,
        0,83,237,
        2,82,239,
        1,81,240,
        1,80,243,
        1,78,246,
        2,77,248,
        0,74,247,
        0,71,247,
        2,73,251,
        0,69,243,
        0,68,239,
        0,68,239,
        3,67,237,
        4,67,234,
        1,63,226,
        2,61,223,
        2,60,219,
        0,58,217,
        0,57,209,
        0,56,205,
        0,56,205,
        0,53,201,
        1,52,197,
        2,51,192,
        2,49,187,
        2,48,185,
        0,46,181,
        0,45,174,
        0,43,171,
        1,42,166,
        0,42,163,
        0,41,159,
        3,40,155,
        3,39,151,
        3,37,145,
        2,36,143,
        0,33,138,
        0,32,135,
        1,30,132,
        0,29,131,
        0,28,127,
        1,25,121,
        1,24,117,
        0,23,113,
        0,21,110,
        0,21,104,
        1,20,99,
        1,18,96,
        0,16,91,
        0,16,91,
        1,15,86,
        0,13,83,
        0,10,76,
        0,7,75,
        1,6,70,
        2,7,65,
        3,6,59,
        3,5,56,
        3,5,56,
        3,3,55,
        2,1,41,
        0,1,21,
        0,0,8
    };

    //void SetSdrInterface(CSdrInterface* ptr){m_pSdrInterface = ptr;}
    void draw();		//call to draw new fft data onto screen plot
    void setRunningState(bool running) { m_Running = running; }
    void setClickResolution(int clickres) { m_ClickResolution = clickres; }
    void setFilterClickResolution(int clickres) { m_FilterClickResolution = clickres; }
    void setFilterBoxEnabled(bool enabled) { m_FilterBoxEnabled = enabled; }
    void setCenterLineEnabled(bool enabled) { m_CenterLineEnabled = enabled; }
    void setTooltipsEnabled(bool enabled) { m_TooltipsEnabled = enabled; }
    void setBookmarksEnabled(bool enabled) { m_BookmarksEnabled = enabled; }

    void setNewFttData(float *fftData, int size);
    void setNewFttData(float *fftData, float *wfData, int size);

    void setCenterFreq(quint64 f);
    void setFreqUnits(qint32 unit) { m_FreqUnits = unit; }

    void setDemodCenterFreq(quint64 f) { m_DemodCenterFreq = f; }

    void setWaterfallPalette(int pal);

    /*! \brief Move the filter to freq_hz from center. */
    void setFilterOffset(qint64 freq_hz)
    {
        m_DemodCenterFreq = m_CenterFreq + freq_hz;
        drawOverlay();
    }
    qint64 getFilterOffset(void)
    {
        return m_DemodCenterFreq - m_CenterFreq;
    }

    int getFilterBw()
    {
        return m_DemodHiCutFreq - m_DemodLowCutFreq;
    }

    void setHiLowCutFrequencies(int LowCut, int HiCut)
    {
        m_DemodLowCutFreq = LowCut;
        m_DemodHiCutFreq = HiCut;
        drawOverlay();
    }

    void getHiLowCutFrequencies(int *LowCut, int *HiCut)
    {
        *LowCut = m_DemodLowCutFreq;
        *HiCut = m_DemodHiCutFreq;
    }

    void setDemodRanges(int FLowCmin, int FLowCmax, int FHiCmin, int FHiCmax, bool symetric);

    /* Shown bandwidth around SetCenterFreq() */
    void setSpanFreq(quint32 s)
    {
        if (s > 0 && s < INT_MAX) {
            m_Span = (qint32)s;
            setFftCenterFreq(m_FftCenter);
        }
        drawOverlay();
    }

    void setHdivDelta(int delta) { m_HdivDelta = delta; }
    void setVdivDelta(int delta) { m_VdivDelta = delta; }

    void setFreqDigits(int digits) { m_FreqDigits = digits>=0 ? digits : 0; }

    /* Determines full bandwidth. */
    void setSampleRate(float rate)
    {
        if (rate > 0.0)
        {
            m_SampleFreq = rate;
            drawOverlay();
        }
    }

    float getSampleRate(void)
    {
        return m_SampleFreq;
    }

    void setFftCenterFreq(qint64 f) {
        qint64 limit = ((qint64)m_SampleFreq + m_Span) / 2 - 1;
        m_FftCenter = qBound(-limit, f, limit);
    }

    int     getNearestPeak(QPoint pt);
    void    setWaterfallSpan(quint64 span_ms);
    quint64 getWfTimeRes(void);
    void    setFftRate(int rate_hz);
    void    clearWaterfall(void);
    bool    saveWaterfall(const QString & filename) const;

    int getWheelConstant() const;
    void setWheelConstant(int WheelConstant);

signals:
    void newCenterFreq(qint64 f);
    void newDemodFreq(qint64 freq, qint64 delta); /* delta is the offset from the center */
    void newLowCutFreq(int f);
    void newHighCutFreq(int f);
    void newFilterFreq(int low, int high);  /* substitute for NewLow / NewHigh */
    void pandapterRangeChanged(float min, float max);
    void newZoomLevel(float level);

public slots:
    // zoom functions
    void resetHorizontalZoom(void);
    void moveToCenterFreq(void);
    void moveToDemodFreq(void);
    void zoomOnXAxis(float level);

    // other FFT slots
    void setFftPlotColor(const QColor color);
    void setFftFill(bool enabled);
    void setPeakHold(bool enabled);
    void setFftRange(float min, float max);
    void setPandapterRange(float min, float max);
    void setWaterfallRange(float min, float max);
    void setPeakDetection(bool enabled, float c);
    void updateOverlay();

    void setPercent2DScreen(int percent)
    {
        m_Percent2DScreen = percent;
        m_Size = QSize(0,0);
        resizeEvent(NULL);
    }

protected:
    //re-implemented widget event handlers
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent( QWheelEvent * event );

private:
    enum eCapturetype {
        NOCAP,
        LEFT,
        CENTER,
        RIGHT,
        YAXIS,
        XAXIS,
        BOOKMARK
    };

    void        drawOverlay();
    void        makeFrequencyStrs();
    int         xFromFreq(qint64 freq);
    qint64      freqFromX(int x);
    void        zoomStepX(float factor, int x);
    qint64      roundFreq(qint64 freq, int resolution);
    quint64     msecFromY(int y);
    void        clampDemodParameters();
    bool        isPointCloseTo(int x, int xr, int delta)
    {
        return ((x > (xr - delta)) && (x < (xr + delta)));
    }
    void getScreenIntegerFFTData(qint32 plotHeight, qint32 plotWidth,
                                 float maxdB, float mindB,
                                 qint64 startFreq, qint64 stopFreq,
                                 float *inBuf, qint32 *outBuf,
                                 qint32 *maxbin, qint32 *minbin);
    void calcDivSize (qint64 low, qint64 high, int divswanted, qint64 &adjlow, qint64 &step, int& divs);

    bool        m_PeakHoldActive;
    bool        m_PeakHoldValid;
    qint32      m_fftbuf[MAX_SCREENSIZE];
    quint8      m_wfbuf[MAX_SCREENSIZE]; // used for accumulating waterfall data at high time spans
    qint32      m_fftPeakHoldBuf[MAX_SCREENSIZE];
    float      *m_fftData;     /*! pointer to incoming FFT data */
    float      *m_wfData;
    int         m_fftDataSize;

    int         m_XAxisYCenter;
    int         m_YAxisWidth;

    eCapturetype    m_CursorCaptured;
    QPixmap     m_2DPixmap;
    QPixmap     m_OverlayPixmap;
    QPixmap     m_WaterfallPixmap;
    QColor      m_ColorTbl[256];
    QSize       m_Size;
    QString     m_Str;
    QString     m_HDivText[HORZ_DIVS_MAX+1];
    bool        m_Running;
    bool        m_DrawOverlay;
    qint64      m_CenterFreq;       // The HW frequency
    qint64      m_FftCenter;        // Center freq in the -span ... +span range
    qint64      m_DemodCenterFreq;
    qint64      m_StartFreqAdj;
    qint64      m_FreqPerDiv;
    bool        m_CenterLineEnabled;  /*!< Distinguish center line. */
    bool        m_FilterBoxEnabled;   /*!< Draw filter box. */
    bool        m_TooltipsEnabled;     /*!< Tooltips enabled */
    bool        m_BookmarksEnabled;   /*!< Show/hide bookmarks on spectrum */
    int         m_DemodHiCutFreq;
    int         m_DemodLowCutFreq;
    int         m_DemodFreqX;		//screen coordinate x position
    int         m_DemodHiCutFreqX;	//screen coordinate x position
    int         m_DemodLowCutFreqX;	//screen coordinate x position
    int         m_CursorCaptureDelta;
    int         m_GrabPosition;
    int         m_Percent2DScreen;

    int         m_FLowCmin;
    int         m_FLowCmax;
    int         m_FHiCmin;
    int         m_FHiCmax;
    bool        m_symetric;

    int         m_HorDivs;   /*!< Current number of horizontal divisions. Calculated from width. */
    int         m_VerDivs;   /*!< Current number of vertical divisions. Calculated from height. */

    float       m_PandMindB;
    float       m_PandMaxdB;
    float       m_WfMindB;
    float       m_WfMaxdB;

    qint64      m_Span;
    float       m_SampleFreq;    /*!< Sample rate. */
    qint32      m_FreqUnits;
    int         m_ClickResolution;
    int         m_FilterClickResolution;
    int         m_WheelConstant;

    int         m_Xzero;
    int         m_Yzero;  /*!< Used to measure mouse drag direction. */
    int         m_FreqDigits;  /*!< Number of decimal digits in frequency strings. */

    QFont       m_Font;         /*!< Font used for plotter (system font) */
    int         m_HdivDelta; /*!< Minimum distance in pixels between two horizontal grid lines (vertical division). */
    int         m_VdivDelta; /*!< Minimum distance in pixels between two vertical grid lines (horizontal division). */

    quint32     m_LastSampleRate;

    QColor      m_FftColor, m_FftFillCol, m_PeakHoldColor;
    bool        m_FftFill;

    float       m_PeakDetection;
    QMap<int,int>   m_Peaks;

    QList< QPair<QRect, qint64> >     m_BookmarkTags;

    // Waterfall averaging
    quint64     tlast_wf_ms;        // last time waterfall has been updated
    quint64     msec_per_wfline;    // milliseconds between waterfall updates
    quint64     wf_span;            // waterfall span in milliseconds (0 = auto)
    int         fft_rate;           // expected FFT rate (needed when WF span is auto)
};

#endif // PLOTTER_H
