#ifndef PUSHBUTTONDELEGATE_H
#define PUSHBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class PushButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PushButtonDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // PUSHBUTTONDELEGATE_H
