#pragma once //MAIN_WINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QCheckBox>
#include <QWebChannel>

#include "defines.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void setupCoordinatesUi();
    void setupMap();
    void setupSignalSlots();

    void extractLatLngAndAddMarker();

    static void writeToFile(const QString& filename, const char* data, qint64 len);
    static void readFromFile(const QString& filename, char* data, qint64 len);

public slots: // called from map
    void onMapMove(double latitude, double longitude);
    void onNewPoint(int index, double latitude, double longitude, int altitude);

signals: // handled in map
    void addPoint(double latitude, double longitude);
    void removeAllPoints();

private:
    Ui::MainWindow *ui = nullptr;

    // map
    QWebChannel mChannel;

    // mission
    std::array<QCheckBox, OWM_MISSION_MAX_POINTS> pointsEnabled;
    std::array<QSpinBox, OWM_MISSION_MAX_POINTS> pointsAltitude;
    std::array<QDoubleSpinBox, OWM_MISSION_MAX_POINTS> pointsLatitude;
    std::array<QDoubleSpinBox, OWM_MISSION_MAX_POINTS> pointsLongitude;
};
