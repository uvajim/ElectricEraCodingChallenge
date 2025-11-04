//
// Created by Jinming Liang on 10/23/25.
//

#ifndef ELECTRICERACODINGCHALLENGE_LIB_H
#define ELECTRICERACODINGCHALLENGE_LIB_H

#include <string>
#include "ChargingNetwork.h"
#include <random>
#include <iostream>
#import <fstream>
#import <sstream>

bool parseFile(const std::string& fileName, ChargingNetwork& chargingNetwork);

bool parseTwoInts(const std::string& input, ChargingNetwork& chargingNetwork);

bool parseReport(const std::string& input, ChargingNetwork& chargingNetwork);

bool isWhitespaceOnly(const std::string& str);

bool computeUptime(const ChargingNetwork& network);

uint64_t randomUID();

#endif //ELECTRICERACODINGCHALLENGE_LIB_H
