#include "utils/logger.h"
#include "utils/utils.h"
#include "utils/concurrent_queue.h"
#include <map>
#include <ostream>
#include <mutex>

struct Balance {
    struct BalVal {
        std::string eq;
        std::string cash_bal;
        std::string upl;
        std::string avail_eq;
    };
    std::map<std::string /* ccy */, BalVal> balval;
    bool inited = false;

    friend std::ostream& operator << (std::ostream& o, const Balance& t) {
        o << "=====Balance=====" << std::endl;
        for (auto& v : t.balval) {
            o << "  " << v.first << " \teq: " << v.second.eq << ", avail: " << v.second.avail_eq
                << ", cash: " << v.second.cash_bal << ", upl: " << v.second.upl << std::endl;
        }
        return o;
    }
};

struct Position {
    struct PosData {
        std::string pos_id;
        std::string inst_id;
        std::string inst_type;
        std::string pos_side;
        std::string avg_px;
        std::string pos;
        std::string ccy;
        uint64_t utime_msec;
    };

    std::map<std::string /*pos_id*/, PosData> posval;

    friend std::ostream& operator << (std::ostream& o, const Position& t) {
        o << "=====Position=====" << std::endl;

        for (auto& pos : t.posval) {
            auto& v = pos.second;
            o << "  - pos: " << pos.first << " " << pos.second.inst_id << "  " << pos.second.inst_type << std::endl;
            o << "    " << v.pos_side << " \t" << v.pos << " \t" << v.avg_px << " \t"
                << v.ccy << "\t" << toDateTimeStr(v.utime_msec) << std::endl;
        }
        return o;
    }
};

struct ProductInfo {
    struct Info {
        std::string inst_id;
        std::string inst_type;
        std::string lot_sz;
        std::string min_sz;
        std::string tick_sz;
        std::string settle_ccy;
        std::string ct_val;
        std::string ct_multi;
    };
    std::map<std::string, Info> data;

    friend std::ostream& operator << (std::ostream& o, const ProductInfo& t) {
        o << "=====Instruments=====" << std::endl;

        for (auto& product : t.data) {
            auto& v = product.second;
            o << "  - " << product.first << " " << v.inst_type << " " << v.settle_ccy << std::endl;
            o << "    lot_sz:" << v.lot_sz << " min_sz:" << v.min_sz << " tick_siz:" << v.tick_sz
                << " ct_val:" << v.ct_val << std::endl;
        }
        return o;
    }
};

struct PublicTradesData {
    std::string inst_id;
    std::string trade_id;
    std::string px;
    std::string sz;
    std::string pos_side;
    uint64_t time_msec;
};

struct PublicTickersData {
    std::string inst_id;
    std::string last_px; // 最新成交价
    std::string last_sz; // 最新成交的数量
    std::string ask_px; // 卖一价
    std::string ask_sz; // 卖一价对应的量
    std::string bid_px; // 买一价
    std::string bid_sz; // 买一价对应的数量
};


class TradesManager {
public:
    void updateBalance(const std::string& ccy, Balance::BalVal&& val);

    void updatePosition(const std::string& pos_id, Position::PosData&& val);

    void updateProductInfo(const std::string& inst_id, ProductInfo::Info&& val);

    void updatePublicTradesData(PublicTradesData&& data);

    void updatePublicTickersData(PublicTickersData&& data);

    void updateOrderStatus(const std::string& clordid, OrderStatus status, const std::string& fill_fx = "");

    void markDataDirty(bool dirty=true);

private:
    std::recursive_mutex mutex_;
    Balance balance_;           // 余额
    Position position_;         // 仓位
    ProductInfo product_info_;  // 产品
};


extern TradesManager g_trades_man;