#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>

class PainterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PainterWidget(QWidget *parent = 0);

signals:

public slots:
private:
    QList<QPair<QPoint, QPoint>> lineList_;
    QPoint beforePoint_;
    void drawPoint(QPainter& painter, QPoint pt);
    void drawLine(QPainter& painter, QPoint lpt, QPoint rpt);

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // PAINTERWIDGET_H
