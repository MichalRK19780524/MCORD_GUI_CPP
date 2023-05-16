#ifndef STATUSICONDELEGATE_H
#define STATUSICONDELEGATE_H

#include <QStyledItemDelegate>

class StatusIconDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StatusIconDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    static const QString NO_DIODE;
    static const QString GREEN_DIODE;
    static const QString YELLOW_DIODE;
    static const QString RED_DIODE;
};

#endif // STATUSICONDELEGATE_H
