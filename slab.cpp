#include "slab.h"

Slab::Slab()
{

}

Slab::Slab(quint16 id)
{
    this->id = id;
}

quint16 Slab::getId() const
{
    return id;
}

void Slab::setId(quint16 newId)
{
    id = newId;
}

quint16 Slab::getRawSetVoltage() const
{
    return rawSetVoltage;
}

void Slab::setRawSetVoltage(quint16 newRawSetVoltage)
{
    rawSetVoltage = newRawSetVoltage;
}

float Slab::getSetVoltage() const
{
    return setVoltage;
}

void Slab::setSetVoltage(float newSetVoltage)
{
    setVoltage = newSetVoltage;
}

quint16 Slab::getRawMeasuredVoltage() const
{
    return rawMeasuredVoltage;
}

void Slab::setRawMeasuredVoltage(quint16 newRawMeasuredVoltage)
{
    rawMeasuredVoltage = newRawMeasuredVoltage;
}

float Slab::getMeasuredVoltage() const
{
    return measuredVoltage;
}

void Slab::setMeasuredVoltage(float newMeasuredVoltage)
{
    measuredVoltage = newMeasuredVoltage;
}

quint16 Slab::getRawTemperature() const
{
    return rawTemperature;
}

void Slab::setRawTemperature(quint16 newRawTemperature)
{
    rawTemperature = newRawTemperature;
}

float Slab::getTemperature() const
{
    return temperature;
}

void Slab::setTemperature(float newTemperature)
{
    temperature = newTemperature;
}

quint16 Slab::getRawCurrent() const
{
    return rawCurrent;
}

void Slab::setRawCurrent(quint16 newRawCurrent)
{
    rawCurrent = newRawCurrent;
}

float Slab::getCurrent() const
{
    return current;
}

void Slab::setCurrent(float newCurrent)
{
    current = newCurrent;
}
