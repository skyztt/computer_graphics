#include "painterwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

PainterWidget::PainterWidget(QWidget *parent) : QWidget(parent)
{
    //lineList_.append({QPoint(0, 0), QPoint(0, 50)});
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

void PainterWidget::drawLine(QPainter &painter, QLine line, DrawLineType drawType)
{
    switch (drawType) {
    case QtSource:
        painter.drawLine(line);
        break;
    case DDA:
        drawLine_DDA(painter, line);
        break;
    case MiddlePoint:
        drawLine_MiddlePoint(painter, line);
        break;
    default:
        break;
    }
}

void PainterWidget::drawLine_DDA(QPainter &painter, QLine line)
{
    float k = (float)line.dy() / line.dx();
    QPoint p1 = line.p1();
    QPoint p2 = line.p2();

    if (line.dx() == 0) {  // 斜率不存在
        for (int i = qMin(p1.y(), p2.y()), j = qMax(p1.y(), p2.y()); i < j; ++i) {
            painter.drawPoint(p1.x(), i);
        }
        return;
    } else if (line.dx() == 0) { // 斜率为0
        for (int i = qMin(p1.x(), p2.x()), j = qMax(p1.x(), p2.x()); i < j; ++i) {
            painter.drawPoint(i, p1.y());
        }
        return;
    }

    if (qAbs(k) <= 1) {
        if (p1.x() > p2.x()) {
            qSwap(p1, p2);
        }

        float currentY = p1.y();
        for (int i = p1.x(), j = p2.x(); i < j; ++i) {
            painter.drawPoint(i, qFloor(currentY));
            currentY += k;
        }
    } else {
        k = 1 / k;
        if (p1.y() > p2.y()) {
            qSwap(p1, p2);
        }

        float currentX = p1.x();
        for (int i = p1.y(), j = p2.y(); i < j; ++i) {
            painter.drawPoint(qFloor(currentX), i);
            currentX += k;
        }
    }
}

void PainterWidget::drawLine_MiddlePoint(QPainter &painter, QLine line)
{
    QPoint p1 = line.p1();
    QPoint p2 = line.p2();

    /*
     * 0 = Ax + By + C
     * A = y2 - y1 = dy
     * B = x1 - x2 = -dx
     * C = x2y1 - x1y2
     */
    // 注意，这里要考虑原点在左上角点，而DDA算法利用斜率不需要考虑原点位置

    int A = p2.y() - p1.y();
    int B = p1.x() - p2.x();
    int C = p2.x() * p1.y() - p1.x() * p2.y();

    int sign = (float)line.dy() * line.dx() > 0 ? 1 : -1;


    if (qAbs(line.dy()) <= qAbs(line.dx())) { // |k| <= 1
        if (A * p1.x() + B * (p1.y() + sign) + C < 0) {
            A = -A;
            B = -B;
            C = -C;
        }

        if (p1.x() > p2.x()) {
            qSwap(p1, p2);
        }
        int increaseB = sign * B;

        float currentY = p1.y();
        float midDiscriminant = 0;
        bool bFirstMidDiscriminant = true;
        for (int i = p1.x(), j = p2.x(); i < j; ++i) {
            painter.drawPoint(i, currentY);
            if (Q_UNLIKELY(bFirstMidDiscriminant)) {
                //midDiscriminant = A + B * 0.5; 原先中点的第一次增量中含有0.5，但是只要判断符号，遂将判别式整体扩大2倍，消除浮点运算
                midDiscriminant = (A << 1) + increaseB;
                bFirstMidDiscriminant = false;
            } else {
                if (midDiscriminant >= 0) {
                    midDiscriminant += A << 1;
                } else {
                    midDiscriminant += (A + increaseB) << 1;
                }
            }
            if (midDiscriminant >= 0) {
                currentY = currentY;
            } else {
                currentY = currentY + sign;
            }

        }
    } else {
        if (A * (p1.x() + sign) + B * p1.y() + C < 0) {
            A = -A;
            B = -B;
            C = -C;
        }

        if (p1.y() > p2.y()) {
            qSwap(p1, p2);
        }

        int increaseA = sign * A;
        float currentX = p1.x();
        float midDiscriminant = 0;
        bool bFirstMidDiscriminant = true;
        for (int i = p1.y(), j = p2.y(); i < j; ++i) {
            painter.drawPoint(currentX, i);
            if (Q_UNLIKELY(bFirstMidDiscriminant)) {
                midDiscriminant = increaseA + (B << 1);
                bFirstMidDiscriminant = false;
            } else {
                if (midDiscriminant >= 0) {
                    midDiscriminant += B << 1;
                } else {
                    midDiscriminant += (increaseA + B) << 1;
                }
            }
            if (midDiscriminant >= 0) {
                currentX = currentX;
            } else {
                currentX = currentX + sign;
            }
        }
    }
}

void PainterWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (QLine line : lineList_) {
        drawPoint(painter, line.p1());
        drawPoint(painter, line.p2());
        drawLine(painter, line, MiddlePoint);
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
