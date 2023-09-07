#include "slab.h"


const std::unique_ptr<QMutex> Slab::mutex(new QMutex);


//Slab::Slab(quint16 id, QScopedPointer<Sipm> master, QScopedPointer<Sipm> slave) : id(id),
//    master(std::move(master)),
//    slave(std::move(slave)
//{
//}

Slab::Slab(int id, const std::shared_ptr<Sipm>& master, const std::shared_ptr<Sipm>& slave):
  id(id),
  master(std::move(master)),
  slave(std::move(slave))
{
}

//Slab::Slab(quint16 id) : id(std::move(id)), master(new Sipm()), slave(new Sipm())
//{
//}

int Slab::getId() const
{
  QMutexLocker locker(mutex.get());
  return id;
}

//void Slab::setId(quint16 newId)
//{
//  QMutexLocker locker(mutex.get());
//  id = newId;
//}

std::shared_ptr<Sipm>& Slab::getMaster()
{
  QMutexLocker locker(mutex.get());
  return master;
}

std::shared_ptr<Sipm>& Slab::getSlave()
{
  QMutexLocker locker(mutex.get());
  return slave;
}

Slab::Slab(Slab &&slab) noexcept {
  this->id = slab.id;
  this->master = std::move(slab.master);
  this->slave = std::move(slab.slave);
}

Slab::Slab(const Slab &slab)
{
  this->id = slab.id;
  this->master = slab.master;
  this->slave = slab.slave;
}

Slab &Slab::operator=(Slab &&slab) noexcept{
    if(this == &slab){
        return *this;
    }
    this->id = slab.id;
    this->master = std::move(slab.master);
    this->slave = std::move(slab.slave);

    return *this;
}

//Slab::Slab(Slab const &slab) {
//    this->id = slab.id;
//    this->master = slab.master;
//    this->slave = slab.slave;
//}

Slab &Slab::operator=(const Slab &slab) {
    if(this == &slab){
        return *this;
    }
    this->id = slab.id;
    this->master = slab.master;
    this->slave = slab.slave;
    return *this;
}

//Slab::~Slab() {
//  QMutexLocker locker(mutex.get());
//}

Slab::Slab(qint16 id, std::shared_ptr<Sipm> master, std::shared_ptr<Sipm> slave) : id(id),
    master(master),
    slave(slave)
{}
