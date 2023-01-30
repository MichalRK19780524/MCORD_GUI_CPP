#ifndef SLUB_H
#define SLUB_H
#include <QtGlobal>

class Slab
{
public:
    Slab();
    Slab(quint16 id);
    quint16 getId() const;
    void setId(quint16 newId);
    quint16 getRawSetVoltage() const;
    void setRawSetVoltage(quint16 newRawSetVoltage);
    float getSetVoltage() const;
    void setSetVoltage(float newSetVoltage);
    quint16 getRawMeasuredVoltage() const;
    void setRawMeasuredVoltage(quint16 newRawMeasuredVoltage);
    float getMeasuredVoltage() const;
    void setMeasuredVoltage(float newMeasuredVoltage);
    quint16 getRawTemperature() const;
    void setRawTemperature(quint16 newRawTemperature);
    float getTemperature() const;
    void setTemperature(float newTemperature);
    quint16 getRawCurrent() const;
    void setRawCurrent(quint16 newRawCurrent);
    float getCurrent() const;
    void setCurrent(float newCurrent);

private:
    quint16 id;
    quint16 rawSetVoltage;
    float setVoltage;
    quint16 rawMeasuredVoltage;
    float measuredVoltage;
    quint16 rawTemperature;
    float temperature;
    quint16 rawCurrent;
    float current;
};

#endif // SLUB_H
