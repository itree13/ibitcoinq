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

void TradesManager::updateProductInfo(const std::string& inst_id, ProductInfo::Info&& val) {
    std::lock_guard lock(mutex_);
    product_info_[inst_id] = std::move(val);
}

void TradesManager::updatePublicTradesData(PublicTradesData&& data) {
    // TODO

   /* if (g_show_trades)
        std::cout << "  - " << info.inst_id << " \t" << info.pos_side << " \t"
        << info.sz << " \t" << info.px << "  \t" << toDateTimeStr(info.ts) << std::endl;*/
}

void TradesManager::updateOrderStatus(const std::string& clordid, OrderStatus status, const std::string& fill_fx) {
    // g_user_data.removeFailedGridOrder(clordid);
}

void TradesManager::markDataDirty(bool dirty) {
    // TODO
}

