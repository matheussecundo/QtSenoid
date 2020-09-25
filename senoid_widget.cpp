#include "senoid_widget.h"
#include <QtWidgets>
#include <QDebug>

SenoidWidget::SenoidWidget(QWidget *parent) : QWidget(parent), timer(this), multiplier(1), current_sample(0)
{
    setMouseTracking(true);

    pen.setStyle(Qt::PenStyle::SolidLine);
    pen.setColor(QColor(0xff, 0, 0));

    timer.callOnTimeout([&]() {
        if (current_sample < (width() - 10)) {
            current_sample += 10;
        } else {
            current_sample = 0;
        }
        update();
    });

    timer.start(16);
}

PositionOnSenoid SenoidWidget::getPositionOnSenoid(qreal x, qreal y) const {
    auto sample_y = samples[current_sample + x];
    sample_y = (sample_y * height() / 2) + height() / 2;
    if (y < sample_y) {
        return PositionOnSenoid::top;
    } else if (y > sample_y) {
        return PositionOnSenoid::bottom;
    }
    return PositionOnSenoid::point;
}

void SenoidWidget::resample(int width, qreal multiplier)
{
    this->multiplier = multiplier;
    samples.resize(width * 2);
    for (int i = 0; i < samples.size(); ++i) {
        samples[i] = static_cast<qreal>(std::sin(i * (2 * M_PI / (width / multiplier))));
    }
}

void SenoidWidget::resizeEvent(QResizeEvent *event)
{
    auto size = event->size();
    if (current_sample >= size.width()) {
        current_sample = 0;
    }
    this->resample(size.width());
}

void SenoidWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto mouse_pos = event->localPos();
    if (mouse_pos.x() >= 0 && mouse_pos.x() < width()) {
        auto position = getPositionOnSenoid(mouse_pos.x(), mouse_pos.y());
        switch (position) {
            case PositionOnSenoid::top:
                qDebug() << mouse_pos << " TOP";
            break;
            case PositionOnSenoid::bottom:
                qDebug() << mouse_pos << " BOTTOM";
            break;
            case PositionOnSenoid::point:
                qDebug() << mouse_pos << " POINT";
            break;
        }
    }
}

void SenoidWidget::paintEvent(QPaintEvent *)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);
    painter.translate(-current_sample, height() / 2);

    for (int i = current_sample; i < width() + current_sample - 1; ++i) {
        painter.drawLine(
            QPointF {i + 0.0, samples[i + 0] * height() / 2},
            QPointF {i + 1.0, samples[i + 1] * height() / 2}
        );
    }
}
