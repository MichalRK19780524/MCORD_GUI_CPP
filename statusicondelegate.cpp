#include "statusicondelegate.h"
#include "statuscolor.h"
#include "widget.h"

#include<QPainter>
#include <QSvgRenderer>

const QString StatusIconDelegate::NO_DIODE {":/images/transparent_circle.svg"};
const QString StatusIconDelegate::GREEN_DIODE {":/images/green_circle.svg"};
const QString StatusIconDelegate::YELLOW_DIODE {":/images/yellow_circle.svg"};
const QString StatusIconDelegate::RED_DIODE {":/images/red_circle.svg"};

StatusIconDelegate::StatusIconDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}


void StatusIconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == Widget::STATUS_COLOR_COLUMN_INDEX)
    {
        QSvgRenderer renderer;
        StatusColor statusColor = index.data().value<StatusColor>();
        switch (statusColor)
        {
            case StatusColor::Transparent:
                renderer.load(NO_DIODE);
                renderer.setAspectRatioMode(Qt::KeepAspectRatio);
                break;
            case StatusColor::Green:
                renderer.load(GREEN_DIODE);
                renderer.setAspectRatioMode(Qt::KeepAspectRatio);
                break;
            case StatusColor::Yellow:
                renderer.load(YELLOW_DIODE);
                renderer.setAspectRatioMode(Qt::KeepAspectRatio);
                break;
            case StatusColor::Red:
                renderer.load(RED_DIODE);
                renderer.setAspectRatioMode(Qt::KeepAspectRatio);
                break;
            default:
                break;
        }

        painter->save();
            renderer.render(painter, option.rect);
        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
