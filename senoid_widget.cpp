#include "senoid_widget.h"
#include <QtWidgets>
#include <QDebug>

SenoidWidget::SenoidWidget(QWidget *parent) : QWidget(parent), timer(this), multiplier(1), velocity(10), current_sample(0)
{
    setMouseTracking(true);

    pen.setStyle(Qt::PenStyle::SolidLine);
    pen.setColor(QColor(0xff, 0, 0));

    timer.callOnTimeout([&]() {
        current_sample = (current_sample + velocity) % width();
        update();
    });

    timer.start(16);
}

PositionOnSenoid SenoidWidget::getPositionOnSenoid(qreal x, qreal y) const {
    auto line = dimensioned_lines[(current_sample + static_cast<int>(x)) % dimensioned_lines.size()];
    auto sample_y = line.y1() + height() / 2;
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
    dimensioned_lines.resize(width);
    for (int i = 0; i < dimensioned_lines.size(); ++i) {
        auto sample0 = std::sin((i + 0) * (2 * M_PI / (width / multiplier)));
        auto sample1 = std::sin((i + 1) * (2 * M_PI / (width / multiplier)));
        dimensioned_lines[i] = QLineF {
            {i + 0.0, sample0 * height() / 2},
            {i + 1.0, sample1 * height() / 2}
        };
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
    painter.drawLines(dimensioned_lines);
    painter.translate(width(), 0);
    painter.drawLines(dimensioned_lines);
}
