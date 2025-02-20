
#include "IconManager.h"

QMap<QString,QIcon*> IconManager::iconMap;

QIcon* IconManager::GetIcon(QString name)
{
    if(!iconMap.contains(name))
    {
        iconMap[name] =new QIcon(GetIconPath(name));
    }
    return iconMap[name];
}

QString IconManager::GetIconPath(QString name)
{
    auto path = FILE_PATH("Source/Application/Editor/icons")+"/"+name.toStdString();
    return QString::fromStdString(path);
}
