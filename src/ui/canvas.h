#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QObject>
#include "widgets/labeldialog.h"
#include <managers/labelmanager.h>
#include <managers/annotationmanager.h>

//QT_BEGIN_NAMESPACE
//namespace Ui { class Canvas; }
//QT_END_NAMESPACE

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(LabelManager *label_manager, AnnotationManager *annotation_manager, QWidget *parent = nullptr);
    QSize sizeUnscaled() const { return this->current_image.size(); }
    qreal getScale() const { return scale; }
    QString annotationDir = "";
    LabelManager *label_manager;
    AnnotationManager *annotation_manager;

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 这两个重载函数对于adjustSize()以及layout等是必须的
    QSize sizeHint() const override { return minimumSizeHint(); }
    QSize minimumSizeHint() const override;

signals:
    void mouseMoved(QPoint pos);
    void newRectangleAnnotated(QRect newRect);
    void removeRectRequest(int idx);

private:
    QPixmap current_image;
    qreal scale = 1.0;
    QPoint mousePos;
    QList<QPoint> curPoints;
    int selectShape(QPoint pos);
    QPoint offsetToCenter();
    QPoint pixelPos(QPoint pos);
    QPoint boundedPixelPos(QPoint pos);
    bool outOfPixmap(QPoint pos);

public slots:
    void loadPixmap(QString file_path);
    void close();

    void setScale(qreal);

};
#endif // CANVAS_H
