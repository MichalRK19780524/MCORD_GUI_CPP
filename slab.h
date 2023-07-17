#ifndef SLUB_H
#define SLUB_H
#include <QtGlobal>
#include <QString>
#include <QMetaType>

#include "sipm.h"

class Slab
{
public:
    Slab();
    Slab(quint16 id);

    Slab(quint16 id, Sipm *master, Sipm *slave);
    quint16 getId() const;
    void setId(quint16 newId);

    Sipm *getMaster() const;
    Sipm *getSlave() const;

private:
    quint16 id = 0;
    Sipm* master;
    Sipm* slave;
};

Q_DECLARE_METATYPE(Slab)
#endif // SLUB_H
