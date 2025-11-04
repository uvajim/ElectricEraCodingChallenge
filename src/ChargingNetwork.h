#ifndef ELECTRICERACODINGCHALLENGE_CHARGINGNETWORK_H
#define ELECTRICERACODINGCHALLENGE_CHARGINGNETWORK_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>
#include <memory>    // <-- for std::unique_ptr
#include <queue>


class StationUptimeReport {
public:
    int stationID;
    int uptime;

    StationUptimeReport(int stationID, int uptime);

    bool operator==(const StationUptimeReport& other) const noexcept { return stationID == other.stationID; }
    bool operator<(const StationUptimeReport& other)  const noexcept { return stationID <  other.stationID; }
};

class ChargerAvailabilityReport {
public:
    uint32_t startTime;
    uint32_t endTime;
    bool up;
    uint64_t reportID;

    ChargerAvailabilityReport(uint32_t startTime, uint32_t endTime, bool up);



    bool operator==(const ChargerAvailabilityReport& other) const noexcept {
        return reportID == other.reportID;
    }

    bool operator<(const ChargerAvailabilityReport& other) const noexcept {
        // You probably want startTime/endTime here, but I'll leave your logic
        return startTime < other.startTime;
    }
};

class Charger {
public:
    int id;
    std::set<ChargerAvailabilityReport*> reports;

    explicit Charger(int id);

    bool operator==(const Charger& other) const noexcept { return id == other.id; }
    bool operator<(const Charger& other)  const noexcept { return id <  other.id; }
};

class ChargingStation {
public:
    int id;
    std::set<Charger*> chargers;

    explicit ChargingStation(int id);

    bool operator==(const ChargingStation& other) const noexcept { return id == other.id; }
    bool operator<(const ChargingStation& other)  const noexcept { return id <  other.id; }
};

// Hashes for value types (still fine to keep)
namespace std {
template<>
struct hash<Charger> {
    size_t operator()(const Charger& c) const noexcept {
        return std::hash<int>{}(c.id);
    }
};
template<>
struct hash<ChargingStation> {
    size_t operator()(const ChargingStation& s) const noexcept {
        return std::hash<int>{}(s.id);
    }
};
} // namespace std

class ChargingNetwork {
public:
    struct CompareReport {
        bool operator()(ChargerAvailabilityReport* a, ChargerAvailabilityReport* b) const {
            return a->startTime > b->startTime; // min-heap
        }
    };
    // OWNERSHIP: these two maps actually own all stations/chargers.
    // The unique_ptr keeps them alive as long as ChargingNetwork exists.
    std::unordered_map<int, std::unique_ptr<ChargingStation>> stationsById;
    std::unordered_map<int, std::unique_ptr<Charger>>         chargersById;

    // RELATIONSHIPS: these reference the owned objects above.
    // Key: ChargingStation* (stable pointer into stationsById)
    // Val: set of Charger*   (stable pointers into chargersById)
    std::unordered_map<ChargingStation*, std::set<Charger*>> network;

    // Reports per Charger*
    std::unordered_map<Charger*, std::priority_queue<
                                 ChargerAvailabilityReport*,
                                 std::vector<ChargerAvailabilityReport*>,
                                 CompareReport>> reports;

    // Helper: get or create station object for a station ID, return pointer to it
    ChargingStation* getOrCreateStation(int stationID) {
        auto it = stationsById.find(stationID);
        if (it != stationsById.end()) {
            return it->second.get();
        }

        auto st = std::make_unique<ChargingStation>(stationID);
        ChargingStation* stPtr = st.get();        // raw pointer to stable heap object
        stationsById[stationID] = std::move(st);  // ChargingNetwork now owns it
        return stPtr;
    }

    // Helper: get or create charger object for a charger ID, return pointer to it
    Charger* getOrCreateCharger(int chargerID) {
        auto it = chargersById.find(chargerID);
        if (it != chargersById.end()) {
            return it->second.get();
        }

        auto ch = std::make_unique<Charger>(chargerID);
        Charger* chPtr = ch.get();
        chargersById[chargerID] = std::move(ch);
        return chPtr;
    }

    // Public API you call from parsing:
    // "Add this charger to this station"
    bool addChargingStation(int stationID, int chargerID) {
        ChargingStation* station = getOrCreateStation(stationID);
        Charger* charger = getOrCreateCharger(chargerID);

        // prevent duplicates (both in station->chargers and in network map)
        if (station->chargers.contains(charger)) {
            return false;
        }

        station->chargers.insert(charger);
        network[station].insert(charger);
        return true;
    }

    // You can keep this stub for now if you'll attach reports later
    bool addChargerAvailabilityReport(int chargerID,
                                      ChargerAvailabilityReport* report) {
        Charger* charger = getOrCreateCharger(chargerID);

        // attach to that charger's report vector
        reports[charger].push(report);

        // also record in the Charger object's own report set if you want
        charger->reports.insert(report);

        return true;
    }
};

#endif // ELECTRICERACODINGCHALLENGE_CHARGINGNETWORK_H
