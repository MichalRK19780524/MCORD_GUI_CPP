#include "simp.h"

Simp::Simp(): status("OK")
{
}

quint16 Simp::getRawSetVoltage() const
{
    return rawSetVoltage;
}

void Simp::setRawSetVoltage(quint16 newRawSetVoltage)
{
    rawSetVoltage = newRawSetVoltage;
}

float Simp::getSetVoltage() const
{
    return setVoltage;
}

void Simp::setSetVoltage(float newSetVoltage)
{
    setVoltage = newSetVoltage;
}

float Simp::getMeasuredVoltage() const
{
    return measuredVoltage;
}

void Simp::setMeasuredVoltage(float newMeasuredVoltage)
{
    measuredVoltage = newMeasuredVoltage;
}

quint16 Simp::getRawTemperature() const
{
    return rawTemperature;
}

void Simp::setRawTemperature(quint16 newRawTemperature)
{
    rawTemperature = newRawTemperature;
}

float Simp::getTemperature() const
{
    return temperature;
}

void Simp::setTemperature(float newTemperature)
{
    temperature = newTemperature;
}

quint16 Simp::getRawCurrent() const
{
    return rawCurrent;
}

void Simp::setRawCurrent(quint16 newRawCurrent)
{
    rawCurrent = newRawCurrent;
}

float Simp::getCurrent() const
{
    return current;
}

void Simp::setCurrent(float newCurrent)
{
    current = newCurrent;
}

QString Simp::getStatus() const
{
    return status;
}

void Simp::setStatus(const QString &newStatus)
{
    status = newStatus;
}

quint16 Simp::getRawCurrentStandardDeviation() const
{
    return rawCurrentStandardDeviation;
}

void Simp::setRawCurrentStandardDeviation(quint16 newRawCurrentStandardDeviation)
{
    rawCurrentStandardDeviation = newRawCurrentStandardDeviation;
}

float Simp::getCurrentStandardDeviation() const
{
    return currentStandardDeviation;
}

void Simp::setCurrentStandardDeviation(float newCurrentStandardDeviation)
{
    currentStandardDeviation = newCurrentStandardDeviation;
}

float Simp::getTemperatureStandardDeviation() const
{
    return temperatureStandardDeviation;
}

void Simp::setTemperatureStandardDeviation(float newTemperatureStandardDeviation)
{
    temperatureStandardDeviation = newTemperatureStandardDeviation;
}

Simp::Simp(quint16 rawSetVoltage, float setVoltage, float measuredVoltage, quint16 rawTemperature, float temperature, quint16 rawCurrent, float current):
    status("OK"),
    rawSetVoltage(std::move(rawSetVoltage)),
    setVoltage(setVoltage),
    measuredVoltage(measuredVoltage),
    rawTemperature(std::move(rawTemperature)),
    temperature(temperature),
    rawCurrent(std::move(rawCurrent)),
    current(current)

{}
