#include "annotationmanager.h"
#include <QMessageBox>
#include <QDebug>

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

void AnnotationManager::toXml(QString xml_path){
    qDebug()<< "Start toXml()...";
    static const char* xml =
        "<annotation>"
        "<folder>VOC2012</folder>"
        "<filename>2007_004275.jpg</filename>"
        "<source>"
            "<database>The VOC2007 Database</database>"
            "<annotation>PASCAL VOC2007</annotation>"
            "<image>flickr</image>"
        "</source>"
        "<size>"
            "<width>500</width>"
            "<height>375</height>"
            "<depth>3</depth>"
        "</size>"
        "<segmented>1</segmented>"

        "</annotation>";
    static const char * char_object =
        "<object>"
            "<name>train</name>"
            "<pose>Unspecified</pose>"
            "<truncated>1</truncated>"
            "<difficult>0</difficult>"
            "<bndbox>"
                "<xmin>1</xmin>"
                "<ymin>89</ymin>"
                "<xmax>441</xmax>"
                "<ymax>318</ymax>"
            "</bndbox>"
        "</object>";
    QString base_name = "";
    QPixmap src_img;
    if(src_img_path != "" && QFile::exists(src_img_path)){
        src_img = QPixmap(src_img_path);
        base_name = src_img_path.split("/").last();
    }
    if (src_img.isNull()) return;
    if (xml_path == ""){
        xml_path = annotation_dir +"/" +base_name.split(".")[0] +".xml";
    }
    TiXmlDocument doc;
    doc.Parse( xml );
    TiXmlDocument object;
    object.Parse(char_object);
    TiXmlElement *root = doc.FirstChildElement();
    root->FirstChildElement("filename")->FirstChild()->SetValue(base_name.toStdString().c_str());
    root->FirstChildElement("size")->FirstChildElement("height")->FirstChild()->SetValue(std::to_string(src_img.height()).c_str());
    root->FirstChildElement("size")->FirstChildElement("width")->FirstChild()->SetValue(std::to_string(src_img.width()).c_str());
    root->FirstChildElement("size")->FirstChildElement("depth")->FirstChild()->SetValue(std::to_string(src_img.depth()/8-1).c_str());
    for(int i = 0; i< objects.length(); i++){
        object.FirstChildElement()->FirstChildElement("name")->FirstChild()->SetValue(objects[i].label.toStdString().c_str());
        object.FirstChildElement()->FirstChildElement("bndbox")->FirstChildElement("xmin")->FirstChild()->SetValue(std::to_string(objects[i].x1).c_str());
        object.FirstChildElement()->FirstChildElement("bndbox")->FirstChildElement("xmax")->FirstChild()->SetValue(std::to_string(objects[i].x2).c_str());
        object.FirstChildElement()->FirstChildElement("bndbox")->FirstChildElement("ymin")->FirstChild()->SetValue(std::to_string(objects[i].y1).c_str());
        object.FirstChildElement()->FirstChildElement("bndbox")->FirstChildElement("ymax")->FirstChild()->SetValue(std::to_string(objects[i].y2).c_str());
        root->InsertEndChild(*object.FirstChild());
    }
    doc.SetTabSize(4);
    doc.SaveFile(xml_path.toStdString().c_str());
    return;
}
