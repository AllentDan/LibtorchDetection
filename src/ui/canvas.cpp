#include "canvas.h"
#include "ui_labeltool.h"

#include <QMouseEvent>
#include <QDebug>
#include <QtWidgets>

void drawRectAnnotation(QPainter &p, const QRect &rect,
                        QColor brushColor, qreal brushAlphaF,
                        QColor penColor, qreal penAlphaF)
{
    p.save();
    brushColor.setAlphaF(brushAlphaF); QBrush brush(brushColor); p.setBrush(brush);
    penColor.setAlphaF(penAlphaF); QPen pen(penColor); p.setPen(pen);
    p.drawRect(rect);
    p.restore();
}


Canvas::Canvas(LabelManager *_label_manager, AnnotationManager *_annotation_manager, QWidget *parent):
    QWidget(parent),
    label_manager(_label_manager),
    annotation_manager(_annotation_manager),
    current_image()
{
    mousePos=QPoint(0,0);
    setMouseTracking(true);
}

void Canvas::paintEvent(QPaintEvent *event){
    if (this->current_image.isNull()){
        QWidget::paintEvent(event);
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.scale(scale, scale);
    painter.translate(offsetToCenter());

    painter.drawPixmap(0,0, this->current_image);
    painter.setPen(QPen(Qt::white));

    if(mode == DRAW){
        for (int i=0;i<annotation_manager->length();i++){
            auto item = (*annotation_manager)[i];
            QRect rect=QRect(item.x1, item.y1, item.x2 -item.x1, item.y2 -item.y1);
            QString label=item.label;
            if (label_manager->hasLabel(label) && (*label_manager)[label].isValid()){
                QColor color = (*label_manager)[label];
                drawRectAnnotation(painter, rect, color, 0.2, color, 0.5);
            }else{
                painter.drawRect(rect);
            }
            QFont font("Helvetica"); font.setFamily("Times"); font.setPixelSize(15);
            painter.setFont(font);
            painter.drawText(rect.topLeft()-QPoint(0,15/2), label);
        }

        // 正在绘制的矩形标注
        if (curPoints.length()>0){
            painter.drawRect(QRect(curPoints[0], curPoints[1]).normalized());
        }
    }else if (mode == SELECT){
        // 已有的矩形标注，淡化显示，选中的除外
        for (int i=0;i<annotation_manager->length();i++){
            if (i==selectedIdx) continue;

            auto item = (*annotation_manager)[i];
            QRect rect=QRect(item.x1, item.y1, item.x2 -item.x1, item.y2 -item.y1);
            QString label=item.label;
            if (label_manager->hasLabel(label) && (*label_manager)[label].isValid()){
                QColor color = (*label_manager)[label];
                drawRectAnnotation(painter, rect, color, 0.1, color, 0.2);
            }else{
                painter.drawRect(rect);
            }
        }
        // 选中的矩形标注，突出显示
        auto selectedItem = (*annotation_manager)[selectedIdx];
        QString selectedLabel = selectedItem.label;
        QRect drawedRect = QRect(selectedItem.x1, selectedItem.y1, selectedItem.x2 -selectedItem.x1, selectedItem.y2 -selectedItem.y1);
        painter.save();
        QColor color = (*label_manager)[selectedLabel];
        color.setAlphaF(0.2); QBrush brush(color); painter.setBrush(brush);
        QPen pen(Qt::white); pen.setStyle(Qt::DashLine); painter.setPen(pen);
        painter.drawRect(drawedRect);
        painter.restore();

        QFont font("Helvetica"); font.setFamily("Times"); font.setPixelSize(15);
        painter.setFont(font);
        painter.drawText(drawedRect.topLeft()-QPoint(0,15/2), selectedLabel);
    }
    painter.end();
    return;
}

void Canvas::mousePressEvent(QMouseEvent *event){
    if (this->current_image.isNull()){
        QWidget::mousePressEvent(event);
        return;
    }
    QPoint pixPos = pixelPos(event->pos());
    QPoint boundedPixPos = boundedPixelPos(event->pos());
    emit mouseMoved(boundedPixPos);
    if (event->button() == Qt::LeftButton){         // 左键开始绘制矩形
        if (curPoints.length()==0){
            if (!outOfPixmap(pixPos)){
                curPoints.push_back(pixPos);
                curPoints.push_back(pixPos);
                update();
            }
        } else if (curPoints.length()==2){
            curPoints[1] = boundedPixPos;
            emit newRectangleAnnotated(QRect(curPoints[0], curPoints[1]).normalized());
            curPoints.clear();
        } else {
            throw "Anomaly length of curPoints of new rectangle";
        }
    }else if (event->button() == Qt::RightButton){  // 右键删除矩形或取消当前正在绘制的矩形
        if (curPoints.length()==0){
            int selectedIdx = selectShape(pixPos);
            if (selectedIdx!=-1)
                emit removeRectRequest(selectedIdx);
        } else if (curPoints.length()==2){
            curPoints.clear();
            update();
        } else {
            throw "Anomaly length of curPoints of new rectangle";
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event){
    if (this->current_image.isNull()){
        QWidget::mouseReleaseEvent(event);
        return;
    }
}


void Canvas::wheelEvent(QWheelEvent *event){
    if (this->current_image.isNull()){
        QWidget::wheelEvent(event);
        return;
    }
    if(event->delta()>0){
        setScale(getScale()*1.1);
    }else{
        setScale(getScale()*0.9);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (this->current_image.isNull()){
        QWidget::mouseMoveEvent(event);
        return;
    }
    mousePos = pixelPos(event->pos());
    QPoint pixPos = boundedPixelPos(event->pos());
    emit mouseMoved(pixPos);
    // 矩形绘制中，移动鼠标说明移动矩形的一个顶点
    if (curPoints.length()==2){
        curPoints[1] = pixPos;
        update();
    } else if (curPoints.length()!=0){
        throw "Anomaly length of curPoints of new rectangle";
    }
}


QSize Canvas::minimumSizeHint() const
{
    if (!this->current_image.isNull())
        return scale * this->current_image.size();
    return QWidget::minimumSizeHint();
}

void Canvas::close() {
    this->current_image = QPixmap();
    curPoints.clear();
//    rectEditing=false;
//    strokeDrawing=false;
//    curStrokes.clear();
    adjustSize();
    update();
}

void Canvas::setScale(qreal new_scale)
{
    scale = new_scale;
    adjustSize();
    update();
}


void Canvas::loadPixmap(QString new_pixmap_path)
{
    this->annotation_manager->src_img_path = new_pixmap_path;
    this->annotation_manager->objects.clear();
    this->current_image = QPixmap(new_pixmap_path);
    QFileInfo info1(new_pixmap_path);
    QString xml_path = this->annotationDir + "/" +info1.baseName() + ".xml";
    if (QFile::exists(xml_path)){
        annotation_manager->fromXml(xml_path);
        emit reloadAnnotations();
    }
    adjustSize();
    update();
}

int Canvas::selectShape(QPoint pos)
{
    return -1;
}

QPoint Canvas::offsetToCenter()
{
    qreal s = scale;
    int w = int(this->current_image.width() * s), h=int(this->current_image.height() * s);
    int aw = this->size().width(), ah = this->size().height();
    int x = aw > w ? int((aw - w) / (2 * s)) : 0;
    int y = ah > h ? int((ah - h) / (2 * s)) : 0;
    return QPoint(x,y);
}

QPoint Canvas::pixelPos(QPoint pos)
{
    return pos / scale - offsetToCenter();
}

QPoint Canvas::boundedPixelPos(QPoint pos)
{
    pos = pos / scale - offsetToCenter();
    pos.setX(std::min(std::max(pos.x(), 0), this->current_image.width()-1));
    pos.setY(std::min(std::max(pos.y(), 0), this->current_image.height()-1));
    return pos;
}

bool Canvas::outOfPixmap(QPoint pos)
{
    int w = this->current_image.width(), h= this->current_image.height();
    return !(0<=pos.x() && pos.x()<w && 0<=pos.y() && pos.y()<h);
}

