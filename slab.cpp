#include "slab.h"

Slab::Slab()
{

}

Slab::Slab(quint16 id, Simp *master, Simp *slave) : id(std::move(id)),
    master(master),
    slave(slave)
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

Simp *Slab::getMaster() const
{
    return master;
}

Simp *Slab::getSlave() const
{
    return slave;
}


