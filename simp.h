#ifndef SIMP_H
#define SIMP_H
#include <QtGlobal>
#include <QString>


class Simp
{
public:
    Simp();
    Simp(quint16 rawSetVoltage, float setVoltage, float measuredVoltage, quint16 rawTemperature, float temperature, quint16 rawCurrent, float current);

    quint16 getRawSetVoltage() const;
    void setRawSetVoltage(quint16 newRawSetVoltage);
    float getSetVoltage() const;
    void setSetVoltage(float newSetVoltage);
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

    QString getStatus() const;
    void setStatus(const QString &newStatus);

    quint16 getRawCurrentStandardDeviation() const;
    void setRawCurrentStandardDeviation(quint16 newRawCurrentStandardDeviation);

    float getCurrentStandardDeviation() const;
    void setCurrentStandardDeviation(float newCurrentStandardDeviation);

    float getTemperatureStandardDeviation() const;
    void setTemperatureStandardDeviation(float newTemperatureStandardDeviation);

private:
    QString status = "OK";
    quint16 rawSetVoltage {0};
    float setVoltage {0.0};
    float measuredVoltage {0.0};
    quint16 rawTemperature {0};
    float temperature {0.0};
    quint16 rawCurrent {0};
    float current {0.0};
    quint16 rawCurrentStandardDeviation {0};
    float currentStandardDeviation {0.0};
    float temperatureStandardDeviation {0.0};
};

#endif // SIMP_H
