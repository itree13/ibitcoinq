#include "utils/logger.h"
#include "utils/utils.h"
#include "utils/concurrent_queue.h"
#include "utils/format_table.h"
#include <map>
#include <ostream>
#include <iomanip>
#include <mutex>

struct Balance {
    struct BalVal {
        std::string eq;         // 币种总权益
        std::string cash_bal;   // 币种余额
        std::string upl;        // 未实现收益
        std::string avail_eq;   // 可用保证金
    };
    std::map<std::string /* ccy */, BalVal> balval;
    bool inited = false;

    friend std::ostream& operator << (std::ostream& o, const Balance& t) {
        FormatTable table("Balance");
        table.addCol("ccy", 8, FormatTable::Align::Left)
            .addCol("eq", 10)
            .addCol("avail eq", 10)
            .addCol("cash", 10)
            .addCol("upl", 4);

        for (auto& v : t.balval) {
            table << v.first << v.second.eq << v.second.avail_eq << v.second.cash_bal << v.second.upl;
        }
        return o << table;
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
        FormatTable table("Position");
        table.addCol("pos", 4, FormatTable::Align::Left)
            .addCol("inst id", 16)
            .addCol("side", 4)
            .addCol("pos", 4)
            .addCol("avg_px", 10)
            .addCol("ccy", 6)
            .addCol("time", 8);

        for (auto& pos : t.posval) {
            auto& v = pos.second;
            table << pos.first << pos.second.inst_id << v.pos_side << v.pos << v.avg_px
                << v.ccy << toDateTimeStr(v.utime_msec);
        }
        return o << table;
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
        std::string ct_val; // 合约面值
        std::string ct_multi; // 合约乘数
    };
    std::map<std::string /* inst_id */, Info> data;

    friend std::ostream& operator << (std::ostream& o, const ProductInfo& t) {
        FormatTable table("Instruments");
        table.addCol("inst id", 16, FormatTable::Align::Left)
            .addCol("ccy", 6)
            .addCol("lot sz", 6)
            .addCol("min sz", 6)
            .addCol("tick sz", 10)
            .addCol("ct val", 6)
            .addCol("ct mul", 6);

        for (auto& pos : t.data) {
            auto& v = pos.second;
            table << v.inst_id << v.settle_ccy << v.lot_sz << v.min_sz << v.tick_sz << v.ct_val << v.ct_multi;
        }
        return o << table;
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

struct PublicTickers {
    struct Data {
        std::string inst_id;
        std::string last_px; // 最新成交价
        std::string last_sz; // 最新成交的数量
        std::string ask_px; // 卖一价
        std::string ask_sz; // 卖一价对应的量
        std::string bid_px; // 买一价
        std::string bid_sz; // 买一价对应的量
        uint64_t utime_msec;
    };
    std::map<std::string /* inst_id */, Data> tickers;

    friend std::ostream& operator << (std::ostream& o, const PublicTickers& t) {
        FormatTable table("Tickers");
        table.addCol("inst id", 16, FormatTable::Align::Left)
            .addCol("last px", 10)
            .addCol("sz", 4)
            .addCol("ask px", 10)
            .addCol("sz", 4)
            .addCol("bid px", 10)
            .addCol("sz", 4);

        for (auto& pos : t.tickers) {
            auto& v = pos.second;
            table << pos.first << v.last_px << v.last_sz << v.ask_px << v.ask_sz << v.bid_px << v.bid_sz;
        }
        return o << table;
    }
};


class TradesManager {
public:
    void updateBalance(const std::string& ccy, Balance::BalVal&& val);

    void updatePosition(const std::string& pos_id, Position::PosData&& val);

    void updateProductInfo(const std::string& inst_id, ProductInfo::Info&& val);

    void updatePublicTickers(PublicTickers::Data&& data);

    void updateOrderStatus(const std::string& clordid, OrderStatus status, const std::string& fill_fx = "");

    void updatePublicTradesData(PublicTradesData&& data);

    void markDataDirty(bool dirty=true);

    void process(const std::string& op);

private:
    std::recursive_mutex mutex_;
    Balance balance_;                   // 余额
    Position position_;                 // 仓位
    ProductInfo product_info_;          // 产品信息
    PublicTickers tickers_data_;    // 行情
};


extern TradesManager g_trades_man;