#pragma once

#include "type.h"
#include <vector>

std::string calcHmacSHA256(const std::string& msg, const std::string& decoded_key);

std::string toDateTimeStr(uint64_t time_msec);
std::string toTimeStr(uint64_t time_msec);

int generateRadomInt(int min_value, int max_value);

std::string generateRandomString(size_t length);

void trimString(string& str);
void splitString(const string& v, std::vector<string>& out, char delim, size_t max_seg = -1);

inline std::string toString(OrderSide side) {
    return (side == OrderSide::Buy) ? "buy" : "sell";
}

inline std::string toShortString(OrderSide side) {
    return (side == OrderSide::Buy) ? "B" : "S";
}

inline std::string toString(OrderPosSide side) {
    return (side == OrderPosSide::Long) ? "Long" : (side == OrderPosSide::Short ? "Short" : "Net");
}

inline std::string toShortString(OrderPosSide side) {
    return (side == OrderPosSide::Long) ? "L" : (side == OrderPosSide::Short ? "S" : "N");
}


inline bool isCloseOrder(OrderPosSide pos_side, OrderSide side) {
    return pos_side == OrderPosSide::Long && side == OrderSide::Sell || pos_side == OrderPosSide::Short && side == OrderSide::Buy;
}

std::string genCliOrdId();
int getCliOrdIdNumber(const std::string& cliordid);
