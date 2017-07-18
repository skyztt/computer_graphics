#include "painterwidget.h"

#include <QMouseEvent>
#include <QPainter>

PainterWidget::PainterWidget(QWidget *parent) : QWidget(parent)
{

}

void PainterWidget::drawPoint(QPainter &painter, QPoint pt)
{
    painter.save();
    QPen oldPen = painter.pen();
    oldPen.setWidth(5);
    painter.setPen(oldPen);

    painter.drawPoint(pt);
    painter.restore();
}

void PainterWidget::drawLine(QPainter &painter, QPoint lpt, QPoint rpt)
{
    painter.drawLine(lpt, rpt);
}

void PainterWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (QPair<QPoint, QPoint> line : lineList_) {
        drawPoint(painter, line.first);
        drawPoint(painter, line.second);
        drawLine(painter, line.first, line.second);
    }
    if (!beforePoint_.isNull()) {
        drawPoint(painter, beforePoint_);
    }

}

void PainterWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (beforePoint_.isNull()) {
        beforePoint_ = event->pos();
    } else {
        lineList_.append({beforePoint_, event->pos()});
        beforePoint_ = QPoint();
    }
    update();
}
