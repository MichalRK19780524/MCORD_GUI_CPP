#ifndef DETECTORTABLEMODEL_H
#define DETECTORTABLEMODEL_H

#include <QAbstractTableModel>
//#include <QMessageBox>
#include "slab.h"

class DetectorTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUM(StatusColor)

public:
    explicit DetectorTableModel(QObject *parent = nullptr);
    explicit DetectorTableModel(const QStringList *headers, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString appendSlab(Slab slab);

    QString updateSlab(Slab slab);

    QModelIndex findIndexOfMasterSlabSetButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabSetButton(quint16 slabId);

    QModelIndex findIndexOfMasterSlabOnButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabOnButton(quint16 slabId);

    QModelIndex findIndexOfMasterSlabOffButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabOffButton(quint16 slabId);

    Slab findSlab(quint16 slabId);
//    QString reloadMasterSlab(Slab* slab);
//    QString reloadSlaveSlab(Slab* slab);

    int findSlabPosition(quint16 slabId);
//
//    [[nodiscard]] QList<Slab> *getSlabs() const;

public:
  [[nodiscard]] QSet<quint16> *getSetId() const;

private:
  QList<Slab> *slabs = nullptr;
  QSet<quint16> *setId = nullptr;
  const QStringList *headers = nullptr;
  static QPixmap createPixmapFromSvgFile(QString path, int width, int height);

signals:
    void slabAppended(quint16 id);
    void slabUpdated(quint16 id);

};

#endif // DETECTORTABLEMODEL_H
