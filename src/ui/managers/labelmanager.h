#ifndef LABELMANAGER_H
#define LABELMANAGER_H
#include <QString>
#include <QMap>
#include "colormanager.h"
#include "../utils/util.h"

class LabelManager
{
public:
    LabelManager();
    bool hasLabel(QString label) const;
    QStringList getLabels() const {return label_color.keys();}
    void addLabel(QString label, QColor color);
    void removeLabel(QString label){label_color.remove(label);}
    void setColor(QString label, QColor color){if(hasLabel(label)) label_color[label] = color;};
    QColor operator[](QString label) const { return label_color[label]; };
    void fromTXT(QString label_path);

private:
    QMap<QString, QColor> label_color;
};

#endif // LABELMANAGER_H
