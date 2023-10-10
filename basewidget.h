#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QObject>
#include <QWidget>

#include "detectortablemodel.h"
#include "lanconnection.h"

class BaseWidget : public QWidget
{
    Q_OBJECT
public:
    static constexpr int SET_COLUMN_INDEX = 4;
    static constexpr int POWER_COLUMN_INDEX = 2;
    static constexpr int STATUS_COLOR_COLUMN_INDEX = 1;
    static constexpr int ID_COLUMN_INDEX = 0;
    static constexpr float MINIMAL_VOLTAGE = 47.0f;
    static constexpr float MINIMAL_OPERATIONAL_VOLTAGE = 50.5f;
    static constexpr int UPDATE_TABLE_TIME = 10000;
    static constexpr quint16 PORT = 5555;
    static constexpr quint16 READ_READY_SERIAL_TIME = 5000;
    static const QStringList HEADERS;
protected:
    enum class SlabState : size_t {
        Detected,
        On,
        Off,
        SetMaster,
        SetSlave,
        SetAll,
        Error
    };

    [[maybe_unused]] static inline size_t qHash(SlabState &key, uint seed) {
        return ::qHash(static_cast<size_t>(key), seed);
    }

    DetectorTableModel *model = nullptr;
    LanConnection *lanConnection = nullptr;

public:
    explicit BaseWidget(LanConnection *lanConnection, QWidget *parent = nullptr);

    void addSetWidgets();

    LanConnection *getLanConnection() const;

signals:

};

#endif // BASEWIDGET_H
