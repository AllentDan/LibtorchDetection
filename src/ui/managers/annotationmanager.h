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
    QString toString(){
        return label+" (" +QString::number(x1)+", " +QString::number(y1)+" ), ("+QString::number(x2)+", " +QString::number(y2)+")";
    }
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
    void toXml(QString xml_path = "");
    void fromXml(QString xml_path = "");
    QString src_img_path = "";
    QString annotation_dir = "";
};

#endif // ANNOTATIONMANAGER_H
