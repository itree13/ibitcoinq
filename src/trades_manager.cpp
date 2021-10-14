#include "trades_manager.h"


TradesManager g_trades_man;


void TradesManager::updateBalance(const std::string& ccy, Balance::BalVal&& val) {
    std::lock_guard lock(mutex_);
    balance_.balval[ccy] = std::move(val);
}

void TradesManager::updatePosition(const std::string& pos_id, Position::PosData&& val) {
    std::lock_guard lock(mutex_);
    if (data.pos == "0")
        position_.posval.erase(pos_id);
    else
        position_.posval[pos_id] == std::move(val);
}