#include "labelmanager.h"

LabelManager::LabelManager()
{
}


void LabelManager::addLabel(QString label, QColor color){
    label_color.insert(label, color);
}

bool LabelManager::hasLabel(QString label){
    return label_color.contains(label);
}

void LabelManager::fromTXT(QString label_path){
    std::ifstream ifs;
    ifs.open(label_path.toStdString(), std::ios::in);
    if (!ifs.is_open())
    {
        std::cout<< "Open "<< label_path.toStdString()<<" file failed.";
        return;
    }
    std::string buf = "";
    while (getline(ifs, buf))
    {
        label_color.insert(QString::fromStdString(buf), ColorManager::randomColor());
    }

}
