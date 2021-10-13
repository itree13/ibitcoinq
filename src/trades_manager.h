#include "utils/logger.h"
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
    std::map<std::string, BalVal> balval;
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


class TradesManager {
public:


private:
    std::recursive_mutex mutex_;
    Balance balance_;           // Óà¶î
    Position position_;         // ²ÖÎ»
    ProductInfo product_info_;  // ²úÆ·
};


extern TradesManager g_trades_man;