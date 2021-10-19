#include "trades_manager.h"
#include <iostream>


TradesManager g_trades_man;


void TradesManager::updateBalance(const std::string& ccy, Balance::BalVal&& val) {
    std::lock_guard lock(mutex_);
    balance_.balval[ccy] = std::move(val);
}

void TradesManager::updatePosition(const std::string& pos_id, Position::PosData&& val) {
    std::lock_guard lock(mutex_);
    if (val.pos == "0")
        position_.posval.erase(pos_id);
    else
        position_.posval[pos_id] = std::move(val);
}

void TradesManager::updateProductInfo(const std::string& inst_id, ProductInfo::Info&& val) {
    std::lock_guard lock(mutex_);
    product_info_.data[inst_id] = std::move(val);
}

void TradesManager::updatePublicTickers(PublicTickers::Data&& data) {
    std::lock_guard lock(mutex_);
    tickers_data_.tickers[data.inst_id] = std::move(data);
}

void TradesManager::updateOrderStatus(const std::string& clordid, OrderStatus status, const std::string& fill_fx) {
    // g_user_data.removeFailedGridOrder(clordid);
}

void TradesManager::updatePublicTradesData(PublicTradesData&& data) {
    // TODO

   /* if (g_show_trades)
        std::cout << "  - " << info.inst_id << " \t" << info.pos_side << " \t"
        << info.sz << " \t" << info.px << "  \t" << toDateTimeStr(info.ts) << std::endl;*/
}

void TradesManager::markDataDirty(bool dirty) {
    // TODO
}

void TradesManager::process(const std::string& op) {
    std::lock_guard lock(mutex_);
    if (op == "show position") {
        std::cout << position_;
    } else if (op == "show balance") {
        std::cout << balance_;
    } else if (op == "show instruments") {
        std::cout << product_info_;
    } else if (op == "show tickers") {
    }
}