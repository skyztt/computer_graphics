#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>

enum DrawLineType {
    QtSource,
    DDA,
    MiddlePoint
};

class PainterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PainterWidget(QWidget *parent = 0);

signals:

public slots:
private:
    QList<QLine> lineList_;
    QPoint beforePoint_;
    void drawPoint(QPainter& painter, QPoint pt);
    void drawLine(QPainter& painter, QLine line, DrawLineType drawType = QtSource);

    void drawLine_DDA(QPainter& painter, QLine line);
    void drawLine_MiddlePoint(QPainter& painter, QLine line);

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // PAINTERWIDGET_H
