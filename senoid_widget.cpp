#include "senoid_widget.h"
#include <QtWidgets>
#include <QDebug>

SenoidWidget::SenoidWidget(QWidget *parent) : QWidget(parent), timer(this), multiplier(1), current_sample(0)
{
    setMouseTracking(true);

    pen.setStyle(Qt::PenStyle::SolidLine);
    pen.setColor(QColor(0xff, 0, 0));

    timer.callOnTimeout([&]() {
        if (current_sample < width()) {
            current_sample++;
        } else {
            current_sample = 0;
        }
        update();
    });

    timer.start(2);
}

PositionOnSenoid SenoidWidget::getPositionOnSenoid(qreal x, qreal y) const {
    auto point = samples[current_sample + x];
    point.setY(point.y() + height() / 2);
    if (y < point.y()) {
        return PositionOnSenoid::top;
    } else if (y > point.y()) {
        return PositionOnSenoid::bottom;
    }
    return PositionOnSenoid::point;
}

void SenoidWidget::resample(int width, int height, qreal multiplier)
{
    this->multiplier = multiplier;
    samples.resize(width * 2);
    for (int i = 0; i < samples.size(); ++i) {
        samples[i] = {
            static_cast<qreal>(i),
            static_cast<qreal>(std::sin(i * (2 * M_PI / (width / multiplier)))) * height / 2
        };
    }
}

void SenoidWidget::resizeEvent(QResizeEvent *event)
{
    auto size = event->size();
    if (current_sample >= size.width()) {
        current_sample = 0;
    }
    this->resample(size.width(), size.height());
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
        painter.drawLine(samples[i], samples[i + 1]);
    }
}
