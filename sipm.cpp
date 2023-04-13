#include "sipm.h"

Sipm::Sipm(): status("OK")
{
}

quint16 Sipm::getRawSetVoltage() const
{
    return rawSetVoltage;
}

void Sipm::setRawSetVoltage(quint16 newRawSetVoltage)
{
    rawSetVoltage = newRawSetVoltage;
}

float Sipm::getSetVoltage() const
{
    return setVoltage;
}

void Sipm::setSetVoltage(float newSetVoltage)
{
    setVoltage = newSetVoltage;
}

float Sipm::getMeasuredVoltage() const
{
    return measuredVoltage;
}

void Sipm::setMeasuredVoltage(float newMeasuredVoltage)
{
    measuredVoltage = newMeasuredVoltage;
}

quint16 Sipm::getRawTemperature() const
{
    return rawTemperature;
}

void Sipm::setRawTemperature(quint16 newRawTemperature)
{
    rawTemperature = newRawTemperature;
}

float Sipm::getTemperature() const
{
    return temperature;
}

void Sipm::setTemperature(float newTemperature)
{
    temperature = newTemperature;
}

quint16 Sipm::getRawCurrent() const
{
    return rawCurrent;
}

void Sipm::setRawCurrent(quint16 newRawCurrent)
{
    rawCurrent = newRawCurrent;
}

float Sipm::getCurrent() const
{
    return current;
}

void Sipm::setCurrent(float newCurrent)
{
    current = newCurrent;
}

QString Sipm::getStatus() const
{
    return status;
}

void Sipm::setStatus(const QString &newStatus)
{
    status = newStatus;
}

quint16 Sipm::getRawCurrentStandardDeviation() const
{
    return rawCurrentStandardDeviation;
}

void Sipm::setRawCurrentStandardDeviation(quint16 newRawCurrentStandardDeviation)
{
    rawCurrentStandardDeviation = newRawCurrentStandardDeviation;
}

float Sipm::getCurrentStandardDeviation() const
{
    return currentStandardDeviation;
}

void Sipm::setCurrentStandardDeviation(float newCurrentStandardDeviation)
{
    currentStandardDeviation = newCurrentStandardDeviation;
}

float Sipm::getTemperatureStandardDeviation() const
{
    return temperatureStandardDeviation;
}

void Sipm::setTemperatureStandardDeviation(float newTemperatureStandardDeviation)
{
    temperatureStandardDeviation = newTemperatureStandardDeviation;
}

Sipm::Sipm(quint16 rawSetVoltage, float setVoltage, float measuredVoltage, quint16 rawTemperature, float temperature, quint16 rawCurrent, float current):
    status("OK"),
    rawSetVoltage(std::move(rawSetVoltage)),
    setVoltage(setVoltage),
    measuredVoltage(measuredVoltage),
    rawTemperature(std::move(rawTemperature)),
    temperature(temperature),
    rawCurrent(std::move(rawCurrent)),
    current(current)
{}
