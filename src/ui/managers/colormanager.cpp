#include "colormanager.h"

ColorManager::ColorManager()
{

}

QColor ColorManager::randomColor(){
    return QColor(qrand()%255, qrand()%255, qrand()%255);
}

QVector<QColor> randomColors(int count){
    QVector<QColor> colors;
    float currentHue = 0.0;
    for (int i = 0; i < count; i++){
        colors.push_back( QColor::fromHslF(currentHue, 1.0, 0.5) );
        currentHue += 0.618033988749895f;
        currentHue = std::fmod(currentHue, 1.0f);
    }
    return colors;
}
