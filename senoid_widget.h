#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QVector>
#include <QtWidgets>

enum class PositionOnSenoid {top, point, bottom};

class SenoidWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SenoidWidget(QWidget *parent = nullptr);
    PositionOnSenoid getPositionOnSenoid(qreal x, qreal y) const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QTimer timer;
    QPen pen;
    QVector<QLineF> dimensioned_lines;

    void resample(int width, qreal multiplier = 1);

    qreal multiplier;
    int velocity;
    int current_sample;
};

#endif // WINDOW_H
