//
// Created by Jinming Liang on 10/23/25.
//

#include "ChargingNetwork.h"

#include "Library.h"


Charger::Charger(int id) : id(id) {
}

ChargingStation::ChargingStation(int id) : id(id) {
}

StationUptimeReport::StationUptimeReport(int stationID, int uptime) : stationID(stationID), uptime(uptime) {
};

ChargerAvailabilityReport::ChargerAvailabilityReport(uint32_t startTime, uint32_t endTime, bool up)
    : startTime(startTime), endTime(endTime), up(up) {
    reportID = randomUID();
}
