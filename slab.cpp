#include "slab.h"

Slab::Slab()
{
}

Slab::Slab(quint16 id, Sipm *master, Sipm *slave) : id(std::move(id)),
    master(master),
    slave(slave)
{
}


Slab::Slab(quint16 id) : id(std::move(id)), master(new Sipm()), slave(new Sipm())
{
}

quint16 Slab::getId() const
{
    return id;
}

void Slab::setId(quint16 newId)
{
    id = newId;
}

Sipm *Slab::getMaster() const
{
    return master;
}

Sipm *Slab::getSlave() const
{
    return slave;
}
