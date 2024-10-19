
#ifndef INSTRENDERER_ICONMANAGER_H
#define INSTRENDERER_ICONMANAGER_H
#include "QIcon"
#include "QMap"
#include "../Engine/Common/Core/PathDefine.h"
class IconManager
{
public:
    static QIcon* GetIcon(QString name);
private:
    static QMap<QString,QIcon*> iconMap;
    static QString GetIconPath(QString name);

};


#endif //INSTRENDERER_ICONMANAGER_H
