#ifndef ANNOTATIONMANAGER_H
#define ANNOTATIONMANAGER_H

#include <QString>
#include <QVector>
#include <QtXml>
#include "../utils/tinyxml.h"

struct detObj{
    QString label;
    int x1;
    int y1;
    int x2;
    int y2;
};

class AnnotationManager
{
public:
    AnnotationManager();
    QVector<detObj> objects;
    void add(detObj obj);
    void remove(int idx);
    size_t length() {return objects.size();};
    detObj operator[](int idx) const { return objects[idx]; };
    void toXml();
    void fromXml(QString xml_path);
};

#endif // ANNOTATIONMANAGER_H
