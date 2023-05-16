#ifndef STATUSCOLOR_H
#define STATUSCOLOR_H

#include <QMetaType>

enum class StatusColor
{
    Transparent, Green, Yellow, Red
};

Q_DECLARE_METATYPE(StatusColor)

#endif // STATUSCOLOR_H
