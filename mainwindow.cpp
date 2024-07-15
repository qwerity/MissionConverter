#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QDesktopServices>
#include <QWebEngineSettings>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>

#define PARAMETERS_FILENAME   "parameters"
#define ENABlED_POINTS_NUMBER 2

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupCoordinatesUi();
    setupMap();
    setupSignalSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCoordinatesUi()
{
    for (auto i = 0; i < OWM_MISSION_MAX_POINTS; ++i)
    {
        pointsEnabled[i].setParent(ui->coordinates);
        pointsEnabled[i].setText(QString("Point %1").arg(i + 1));
        ui->coordinatesGridLayout->addWidget(&pointsEnabled[i], i + 1, 0, 1, 1);

        pointsLatitude[i].setParent(ui->coordinates);
        pointsLatitude[i].setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        pointsLatitude[i].setDisabled(true);
        pointsLatitude[i].setDecimals(7);
        pointsLatitude[i].setRange(-180.0, 180.0);
        ui->coordinatesGridLayout->addWidget(&pointsLatitude[i], i + 1, 1, 1, 1);

        pointsLongitude[i].setParent(ui->coordinates);
        pointsLongitude[i].setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        pointsLongitude[i].setDisabled(true);
        pointsLongitude[i].setDecimals(7);
        pointsLongitude[i].setRange(-180.0, 180.0);
        ui->coordinatesGridLayout->addWidget(&pointsLongitude[i], i + 1, 2, 1, 1);

        pointsAltitude[i].setParent(ui->coordinates);
        pointsAltitude[i].setDisabled(true);
        pointsAltitude[i].setReadOnly(true);
        pointsAltitude[i].setRange(-1000, 3000);
        ui->coordinatesGridLayout->addWidget(&pointsAltitude[i], i + 1, 3, 1, 1);

        connect(&pointsEnabled[i], &QCheckBox::toggled, [i, this](bool checked)
        {
            if (0 != i && ! pointsEnabled[i - 1].isChecked())
            {
                pointsEnabled[i].setChecked(false);
                const auto message = QString("You have to activate (%1) to activate (%2) point").arg(pointsEnabled[i - 1].text(), pointsEnabled[i].text());
                ui->statusbar->showMessage(message);
                return;
            }
            if (OWM_MISSION_MAX_POINTS - 1 != i && pointsEnabled[i + 1].isChecked())
            {
                for (auto j = i; j < OWM_MISSION_MAX_POINTS; ++j)
                {
                    pointsEnabled[j].setChecked(false);
                    pointsLatitude[j].setEnabled(false);
                    pointsLongitude[j].setEnabled(false);
                    pointsAltitude[j].setEnabled(false);
                }
            }
            pointsLatitude[i].setEnabled(checked);
            pointsLongitude[i].setEnabled(checked);
            pointsAltitude[i].setEnabled(checked);
        });

        if (i < ENABlED_POINTS_NUMBER)
        {
            pointsEnabled[i].setChecked(true);
        }
        else
        {
            pointsEnabled[i].setChecked(false);
        }

    }
    pointsEnabled[0].setDisabled(true);
}

void MainWindow::setupMap()
{
    auto page = ui->mapView->page();
    page->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);
    page->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessFileUrls, true);

    connect(page, &QWebEnginePage::featurePermissionRequested, [this, page](const QUrl &securityOrigin, QWebEnginePage::Feature feature)
    {
        if (QWebEnginePage::Geolocation != feature) { return; }

        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle("Location Permission");
        msgBox.setText(tr("Do you want to send your current location to this website?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if (QMessageBox::Yes == msgBox.exec())
        {
            page->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
        }
        else
        {
            page->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
        }
    });


    mChannel.registerObject(QStringLiteral("MainWindow"), this);
    ui->mapView->page()->setWebChannel(&mChannel);

    auto htmlPath = QString("%1/map/map.html").arg(QApplication::applicationDirPath());
    ui->mapView->setUrl(QUrl::fromLocalFile(htmlPath));
}

void MainWindow::writeToFile(const QString& filename, const char *data, qint64 len)
{
    QFile out(filename);

    out.open(QIODevice::WriteOnly | QIODevice::Truncate);
    out.write(data, len);
    out.flush();
    out.close();
}

void MainWindow::readFromFile(const QString &filename, char *data, qint64 len)
{
    QFile in(filename);
    in.open(QIODevice::ReadOnly);
    in.read(data, len);
    in.flush();
    in.close();
}

void MainWindow::setupSignalSlots()
{
    connect(ui->generateParameters, &QPushButton::clicked, [this]
    {
        OWMission mission{};
        uint i = 0;
        for (; i < OWM_MISSION_MAX_POINTS; ++i)
        {
            if (! pointsEnabled[i].isChecked()) { break; }

            mission.points[i] =
                {
                    .latitude = static_cast<int32_t>(pointsLatitude[i].value() * 1e7),
                    .longitude = static_cast<int32_t>(pointsLongitude[i].value() * 1e7),
                    .altitudeInCM = static_cast<uint32_t>(pointsAltitude[i].value() * 100),
            };
            ++mission.usedPointsNumber;
        }
        mission.home = mission.points[i - 1];

        OWMParameters parameters =
        {
            .mission = mission,
            .startDelayInSec = ui->startDelayInSec->value(),
            .fbwaMotorPower = static_cast<float>(ui->fbwaMotorPower->value()),
            .stabilizeMotorPower = static_cast<float>(ui->stabilizeMotorPower->value()),
            .flightHeightInMeter = static_cast<float>(ui->flightHeight->value()),
            .flightHeightOffsetInMeter = static_cast<float>(ui->flightHeightOffset->value()),
            .wpRadiusInMeter = ui->wpRadius->value(),
            .stabilizeFlightTime = ui->stabilizeFlightTime->value(),
            .gpsMaxWaitTime = ui->gpsMaxWaitTime->value(),
            .enableLogging = ui->enableLogging->isChecked(),
            .removeParametersFileAfterRead = ui->removeParametersFileAfterRead->isChecked(),
        };

        writeToFile(PARAMETERS_FILENAME, reinterpret_cast<char *>(&parameters), sizeof(parameters));
        ui->statusbar->showMessage(QString("parameters successfully saved to %1 file").arg(PARAMETERS_FILENAME));
        QDesktopServices::openUrl(QUrl::fromLocalFile("."));

        OWMParameters test{};
        readFromFile(PARAMETERS_FILENAME, reinterpret_cast<char *>(&test), sizeof(test));
        test.stabilizeFlightTime = 0;
    });

    connect(ui->pointLatLng, &QLineEdit::returnPressed, this, &MainWindow::extractLatLngAndAddMarker);
}

void MainWindow::onMapMove(double latitude, double longitude)
{
    ui->statusbar->showMessage(QString("latitude: %1, longitude: %2").arg(QString::number(latitude, 'f', 7), QString::number(longitude, 'f', 7)));
}

void MainWindow::onNewPoint(int index, double latitude, double longitude, int altitude)
{
    if (OWM_MISSION_MAX_POINTS > index && pointsEnabled[index].isChecked())
    {
        pointsLatitude[index].setValue(latitude);
        pointsLongitude[index].setValue(longitude);
        pointsAltitude[index].setValue(altitude);
    }
}

void MainWindow::extractLatLngAndAddMarker()
{
    auto latlngList = ui->pointLatLng->text().split(',');

    if (2 != latlngList.size()) { return; }

    bool latTest{false};
    bool lngTest{false};

    auto lat = latlngList[0].toDouble(&latTest);
    auto lng = latlngList[1].toDouble(&lngTest);
    if (! latTest || ! lngTest) { return; }

    emit addPoint(lat, lng);
}
