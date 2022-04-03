#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>
#include <QIcon>

class ColorManager
{
public:
    ColorManager();
    static QColor randomColor();
    static QVector<QColor> randomColors(int count);
    static QIcon iconFromColor(QColor color, QSize size = QSize(16,16));
};

#endif // COLORMANAGER_H
