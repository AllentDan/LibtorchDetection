#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>

class ColorManager
{
public:
    ColorManager();
    static QColor randomColor();
    static QVector<QColor> randomColors(int count);
};

#endif // COLORMANAGER_H
