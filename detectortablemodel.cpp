#include "detectortablemodel.h"
#include "slab.h"

#include <QMessageBox>

DetectorTableModel::DetectorTableModel(QObject *parent)
    : QAbstractTableModel(parent), slabs(new QList<Slab*>), setId(new QSet<quint16>)
{
}

DetectorTableModel::DetectorTableModel(const QStringList *headers, QObject *parent) : QAbstractTableModel(parent),
    headers(headers), slabs(new QList<Slab*>), setId(new QSet<quint16>)
{}

QVariant DetectorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0 :
                return QString("Slab No.");
            case 1 :
                return QString("Type");
            case 2:
                return QString("Status");
            case 3:
                return QString("Set SiPM Volt.");
            case 4:
                return QString("Power");
            case 5:
                return QString("U[V]");
            case 6:
                return QString("I[nA]");
            case 7:
                return QString("T[C]");
            }
    }
    return QVariant();
}

int DetectorTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return  2 * slabs->size();
}

int DetectorTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 8;
}

QVariant DetectorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() < 0 || index.row() >= 2 * slabs->size())
        return QVariant();
    int row = index.row();
    Slab * slab = slabs->at(row/2);
    Sipm * sipm = nullptr;
    if(row%2 == 0)
    {
        sipm = slab->getMaster();
    }
    else
    {
        sipm = slab->getSlave();
    }

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
            case 0:
                return slab->getId();
            case 1:
                if(row%2 == 0)
                {
                    return QString("Master");
                }
                else
                {
                    return QString("Slave");
                }
            case 2:
                return sipm->getStatus();
            case 3:
                return QVariant();
            case 4:
                return QVariant();
            case 5:
                return sipm->getMeasuredVoltage();
            case 6:
                return sipm->getCurrent();
            case 7:
                return sipm->getTemperature();
        }
    }
    return QVariant();
}

QString DetectorTableModel::appendSlab(Slab* slab)
{
    const int rowIndex = 2 * slabs->size();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex + 1);
    quint16 id = slab->getId();
    if(setId->contains(id))
    {
        return QString("Slab number %1 has already been added.").arg(id);
    }
    else
    {
        slabs->append(slab);
        setId->insert(id);
    }
    endInsertRows();
    return "OK";
}
