//
// Created by Jimmy on 10/23/25.
//

#include "Library.h"
#include "ChargingNetwork.h"
#include <cctype>
#include <algorithm>

bool parseFile(const std::string& fileName, ChargingNetwork& chargingNetwork) {

    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "File " << fileName << " not found." << std::endl;
        return false;
    }
    std::string line;
    while(getline(file, line)) {
        std::istringstream iss(line);
        std::vector<int> tokens;
        std::string token;

            if (line == "[Stations]") {
                continue;
            }

            if (isWhitespaceOnly(line)) {
                continue;
            }

            if (line == "[Charger Availability Reports]") {
                break;
            }

            parseTwoInts(line, chargingNetwork);
    }

    while (getline(file, line)) {
        auto successful = parseReport(line, chargingNetwork);
        if (!successful) return false;
    }

    return true;
};

bool parseTwoInts(const std::string &input, ChargingNetwork& chargingNetwork) {
    std::istringstream iss(input);
    int stationID, chargerID;

    if (!(iss >> stationID >> chargerID)) {
        std::cerr << "This is not a valid entry" << std::endl;
        return false;
    }

    chargingNetwork.addChargingStation(stationID, chargerID);

    return true;
}

bool parseReport(const std::string& input, ChargingNetwork& chargingNetwork) {
    std::istringstream iss(input);
    int chargerID, startTime, endTime;
    bool up;

    // Expect exactly four tokens
    if (!(iss >> chargerID >> startTime >> endTime >> std::boolalpha >> up)) {
        std::cerr << "Invalid input format: " << input << std::endl;
        return false;
    }

    if (endTime < startTime) {
        std::cerr << "A report for " << chargerID << " contains an end time that is before the start time. "<< std::endl;
        return false;
    }

    // Create and attach the report
    auto* report = new ChargerAvailabilityReport(startTime, endTime, up);
    chargingNetwork.addChargerAvailabilityReport(chargerID, report);

    return true;
}

bool isWhitespaceOnly(const std::string& str) {
    return std::ranges::all_of(str,
                               [](unsigned char c){ return std::isspace(c); });
}

bool computeUptime(const ChargingNetwork& chargingNetwork) {
    // total up seconds of up/down per charger ID
    std::unordered_map<int, uint64_t> uptimes;
    std::unordered_map<int, uint64_t> downtimes;
    std::vector<StationUptimeReport> finalResult;
    Charger* prevChargerPtr = nullptr;

    for (const auto& [chargerPtr, heap] : chargingNetwork.reports) {
        auto tempHeap = heap;
        if (tempHeap.empty()) {
            continue;
        }

        // Pull the first interval
        ChargerAvailabilityReport* prev = tempHeap.top();
        tempHeap.pop();

        // Initialize with the first interval's own duration
        uint64_t upTotal   = 0;
        uint64_t downTotal = 0;

        auto addInterval = [&](ChargerAvailabilityReport* r) {
            uint64_t duration = (r->endTime > r->startTime)
                              ? (r->endTime - r->startTime)
                              : 0;
            if (r->up) {
                upTotal += duration;
            } else {
                downTotal += duration;
            }
        };
        addInterval(prev);
        while (!tempHeap.empty()) {
            ChargerAvailabilityReport* curr = tempHeap.top();
            tempHeap.pop();

            if (prevChargerPtr == chargerPtr && prev->endTime < curr->startTime) {
                std::cerr << "This is an invalid report since another report has an overlapping time slot on the same charger." << std::endl;
                return false;
            }

            if (curr->startTime > prev->endTime) {
                const uint64_t gap = curr->startTime - prev->endTime;
                downTotal += gap;

            }
            addInterval(curr);
            prev = curr;
            ChargerAvailabilityReport* next = tempHeap.top();


        }
        uptimes[chargerPtr->id]   += upTotal;
        downtimes[chargerPtr->id] += downTotal;
        prevChargerPtr = chargerPtr;
    }

    for (const auto& [stationPtr, chargerSet] : chargingNetwork.network) {
        uint64_t stationUp   = 0;
        uint64_t stationDown = 0;
        for (Charger* ch : chargerSet) {
            stationUp   += uptimes[ch->id];
            stationDown += downtimes[ch->id];
        }
        uint64_t total = stationUp + stationDown;
        int stationUptimePercent = (total == 0)
            ? 0
            : static_cast<int>(
                  std::floor(
                      (static_cast<double>(stationUp) / static_cast<double>(total))
                      * 100.0
                  )
              );
        finalResult.emplace_back(stationPtr->id, stationUptimePercent);
    }
    std::sort(finalResult.begin(), finalResult.end());
    for (const auto& stationID: finalResult) {
        std::cout << stationID.stationID << " " << stationID.uptime<< std::endl;
    }
    return true;
}

uint64_t randomUID() {
    static std::mt19937_64 gen(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    return dist(gen);
};
