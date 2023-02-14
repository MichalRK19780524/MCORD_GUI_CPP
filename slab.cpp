#include "slab.h"

Slab::Slab()
{
    status = "OK";
}

Slab::Slab(quint16 id, Simp *master, Simp *slave) : id(std::move(id)),
    master(master),
    slave(slave)
{
    status = "OK";
}

quint16 Slab::getId() const
{
    return id;
}

void Slab::setId(quint16 newId)
{
    id = newId;
}

Simp *Slab::getMaster() const
{
    return master;
}

Simp *Slab::getSlave() const
{
    return slave;
}

QString Slab::getStatus() const
{
    return status;
}

void Slab::setStatus(const QString &newStatus)
{
    status = newStatus;
}


