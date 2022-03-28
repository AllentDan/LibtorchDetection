#include "annotationmanager.h"
#include <QMessageBox>

AnnotationManager::AnnotationManager()
{

}

void AnnotationManager::add(detObj obj){
    objects.push_back(obj);
}

void AnnotationManager::remove(int idx){
    if (idx<objects.length()){
        objects.remove(idx);
    }
}


void AnnotationManager::fromXml(QString xml_path){
    TiXmlDocument doc;
    objects.clear();

    if (!doc.LoadFile(xml_path.toStdString().c_str()))
    {
        QMessageBox::information(NULL, "Load xml error", doc.ErrorDesc());
    }


    TiXmlElement *root = doc.FirstChildElement();

    if (root == NULL)
    {
        QMessageBox::information(NULL, "Load xml error", "Failed to load file: No root element.");
        doc.Clear();
    }


    for (TiXmlElement *elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
        std::string elemName = elem->Value();
        std::string name = "";

        if (strcmp(elemName.data(), "object") == 0)
        {
            for (TiXmlNode *object = elem->FirstChildElement(); object != NULL; object = object->NextSiblingElement())
            {
                if (strcmp(object->Value(), "name") == 0)
                {
                    name = object->FirstChild()->Value();
                }

                if (strcmp(object->Value(), "bndbox") == 0)
                {
                    detObj obj;
                    TiXmlElement * xmin = object->FirstChildElement("xmin");
                    TiXmlElement * ymin = object->FirstChildElement("ymin");
                    TiXmlElement * xmax = object->FirstChildElement("xmax");
                    TiXmlElement * ymax = object->FirstChildElement("ymax");

                    obj.x1 = atoi(std::string(xmin->FirstChild()->Value()).c_str());
                    obj.x2 = atoi(std::string(xmax->FirstChild()->Value()).c_str());
                    obj.y1 = atoi(std::string(ymin->FirstChild()->Value()).c_str());
                    obj.y2 = atoi(std::string(ymax->FirstChild()->Value()).c_str());
                    obj.label = QString::fromStdString(name);
                    objects.push_back(obj);
                }

            }
        }
    }
    doc.Clear();
}
