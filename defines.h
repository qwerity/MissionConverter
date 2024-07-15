#pragma once // MISSION_DATA_CONVERTER_DEFINES_H

#include <cstdint>

#define OWM_MISSION_MAX_POINTS (4)

struct GeoCoordinates
{
    int32_t latitude{0};
    int32_t longitude{0};
    uint32_t altitudeInCM{0}; // in CM
};

struct OWMission
{
    GeoCoordinates home;
    std::array<GeoCoordinates, OWM_MISSION_MAX_POINTS> points;
    uint32_t usedPointsNumber{0};
};

struct OWMParameters
{
    OWMission mission;
    int startDelayInSec{300};
    float fbwaMotorPower{90.0f};
    float stabilizeMotorPower{60.0f};
    float flightHeightInMeter{150.0f};
    float flightHeightOffsetInMeter{5.0f};
    int wpRadiusInMeter{5};
    int stabilizeFlightTime{60};
    int gpsMaxWaitTime{20};
    bool enableLogging{false};
    bool removeParametersFileAfterRead{true};
};