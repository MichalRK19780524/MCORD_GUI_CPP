#include "sipm.h"

const std::unique_ptr<QMutex> Sipm::mutex = std::make_unique<QMutex>();

Sipm::Sipm(): status("OK")
{
}

quint16 Sipm::getRawSetVoltage() const
{
  QMutexLocker locker(&*mutex);
  return rawSetVoltage;
}

void Sipm::setRawSetVoltage(quint16 newRawSetVoltage)
{
  QMutexLocker locker(&*mutex);
  rawSetVoltage = newRawSetVoltage;
}

float Sipm::getSetVoltage() const
{
  QMutexLocker locker(&*mutex);
  return setVoltage;
}

void Sipm::setSetVoltage(float newSetVoltage)
{
  QMutexLocker locker(&*mutex);
  setVoltage = newSetVoltage;
}

float Sipm::getMeasuredVoltage() const
{
  QMutexLocker locker(&*mutex);
  return measuredVoltage;
}

void Sipm::setMeasuredVoltage(float newMeasuredVoltage)
{
  QMutexLocker locker(&*mutex);
  measuredVoltage = newMeasuredVoltage;
}

quint16 Sipm::getRawTemperature() const
{
  QMutexLocker locker(&*mutex);
  return rawTemperature;
}

void Sipm::setRawTemperature(quint16 newRawTemperature)
{
  QMutexLocker locker(&*mutex);
  rawTemperature = newRawTemperature;
}

float Sipm::getTemperature() const
{
  QMutexLocker locker(&*mutex);
  return temperature;
}

void Sipm::setTemperature(float newTemperature)
{
  QMutexLocker locker(&*mutex);
  temperature = newTemperature;
}

quint16 Sipm::getRawCurrent() const
{
  QMutexLocker locker(&*mutex);
  return rawCurrent;
}

void Sipm::setRawCurrent(quint16 newRawCurrent)
{
  QMutexLocker locker(&*mutex);
  rawCurrent = newRawCurrent;
}

float Sipm::getCurrent() const
{
  QMutexLocker locker(&*mutex);
  return current;
}

void Sipm::setCurrent(float newCurrent)
{
  QMutexLocker locker(&*mutex);
  current = newCurrent;
}

QString Sipm::getStatus() const
{
  QMutexLocker locker(&*mutex);
  return status;
}

void Sipm::setStatus(const QString &newStatus)
{
  QMutexLocker locker(&*mutex);
  status = newStatus;
}

quint16 Sipm::getRawCurrentStandardDeviation() const
{
  QMutexLocker locker(&*mutex);
  return rawCurrentStandardDeviation;
}

void Sipm::setRawCurrentStandardDeviation(quint16 newRawCurrentStandardDeviation)
{
  QMutexLocker locker(&*mutex);
  rawCurrentStandardDeviation = newRawCurrentStandardDeviation;
}

float Sipm::getCurrentStandardDeviation() const
{
  QMutexLocker locker(&*mutex);
  return currentStandardDeviation;
}

void Sipm::setCurrentStandardDeviation(float newCurrentStandardDeviation)
{
  QMutexLocker locker(&*mutex);
  currentStandardDeviation = newCurrentStandardDeviation;
}

float Sipm::getTemperatureStandardDeviation() const
{
  QMutexLocker locker(&*mutex);
  return temperatureStandardDeviation;
}

void Sipm::setTemperatureStandardDeviation(float newTemperatureStandardDeviation)
{
  QMutexLocker locker(&*mutex);
  temperatureStandardDeviation = newTemperatureStandardDeviation;
}

StatusColor Sipm::getStatusColor() const
{
  QMutexLocker locker(&*mutex);
  return statusColor;
}

void Sipm::setStatusColor(StatusColor newStatusColor)
{
  QMutexLocker locker(&*mutex);
  statusColor = newStatusColor;
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
Sipm::~Sipm() {
  QMutexLocker locker(&*mutex);
}
