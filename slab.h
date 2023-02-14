#ifndef SLUB_H
#define SLUB_H
#include <QtGlobal>
#include <QString>

#include "simp.h"

class Slab
{
public:
    Slab();
    Slab(quint16 id, Simp *master, Simp *slave);
    quint16 getId() const;
    void setId(quint16 newId);

    Simp *getMaster() const;
    Simp *getSlave() const;

    QString getStatus() const;
    void setStatus(const QString &newStatus);

private:
    quint16 id = 0;
    Simp* master;
    Simp* slave;
    QString status;
};

#endif // SLUB_H
