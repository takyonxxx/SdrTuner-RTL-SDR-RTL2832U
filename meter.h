#ifndef METER_H
#define METER_H

#include <QtGui>
#include <QFrame>
#include <QImage>

class CMeter : public QFrame
{
    Q_OBJECT

public:
    explicit CMeter(QWidget *parent = 0);
    explicit CMeter(float min_level = -100.0, float max_level = 10.0,
                    QWidget *parent = 0);
    ~CMeter();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setMin(float min_level);
    void setMax(float max_level);
    void setRange(float min_level, float max_level);

    void draw();
    void UpdateOverlay(){DrawOverlay();}

public slots:
    void setLevel(float dbfs);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent* event);

private:
    void DrawOverlay();
    QPixmap m_2DPixmap;
    QPixmap m_OverlayPixmap;
    QSize m_Size;
    QString m_Str;
    int m_Slevel;
    int m_dBm;

    float d_alpha_decay;
    float d_alpha_rise;
};

#endif // METER_H
