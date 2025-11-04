#include <iostream>
#include <filesystem>
#include "src/Library.h"
#include "src/ChargingNetwork.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main(int argc, char* argv[]) {
    //TODO: 1. accept a file parameter
    //      2. round down properly
    //      3. error handling and exiting

    if (argc < 2) {
        std::cerr << "Please provide a file" << std::endl;
        return 1;
    }

    if (argc > 2) {
        std::cerr << "Too many arguments" << std::endl;
        return 1;
    }

    auto chargingNetwork = ChargingNetwork();
    const auto successfullyParsed = parseFile(argv[1], chargingNetwork);
    if (!successfullyParsed) {
        return 1;
    }
    const auto successfullyComputed = computeUptime(chargingNetwork);
    if (!successfullyComputed) {
        return 1;
    }

    return 0;
}