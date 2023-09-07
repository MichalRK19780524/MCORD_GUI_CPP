#ifndef SLAB_H
#define SLAB_H

#include <QMetaType>
#include <QMutex>
#include <QString>
#include <QtGlobal>
#include <memory>

#include "sipm.h"

class Slab {
public:
    Slab() = default;

    Slab(Slab &&slab) noexcept ;

    Slab(const Slab &slab);

    Slab & operator=(Slab && slab) noexcept ;

    Slab & operator=(const Slab & slab);

    virtual ~Slab() = default;
    //  explicit Slab(quint16 id);

    static const std::unique_ptr<QMutex> mutex;

    Slab(int id, const std::shared_ptr<Sipm> &master, const std::shared_ptr<Sipm> &slave);

    Slab(qint16 id, std::shared_ptr<Sipm> master, std::shared_ptr<Sipm> slave);

    [[nodiscard]] int getId() const;
//  void setId(quint16 newId);

    [[nodiscard]] std::shared_ptr<Sipm> &getMaster();

    [[nodiscard]] std::shared_ptr<Sipm> &getSlave();

private:
    qint16 id = -1;
    std::shared_ptr<Sipm> master;
    std::shared_ptr<Sipm> slave;
};

Q_DECLARE_METATYPE(Slab)

#endif // SLAB_H
