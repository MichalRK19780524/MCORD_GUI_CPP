#include "pushbuttondelegate.h"
#include <QApplication>

PushButtonDelegate::PushButtonDelegate()
{

}


void PushButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 3)
    {
        QStyleOptionButton buttonOption;
        QRect r = option.rect;
        buttonOption.rect = r;
        buttonOption.text = "Set";
        buttonOption.state = QStyle::State_Enabled;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    }
    else
    {
         QStyledItemDelegate::paint(painter, option, index);
    }

}

bool PushButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if( event->type() == QEvent::MouseButtonPress )
    {
         QStyleOptionButton buttonOption;
//         option.
    }
}
