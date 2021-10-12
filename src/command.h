#pragma once

#include "type.h"
#include <deque>

class Command {
public:
    struct Request {
        std::string id;
        std::string op;
        std::string data;
    };

    struct Response {
        std::string id;
        std::string op;
        int code;
        std::string msg;
        std::string data;
    };

    static Request makeLoginReq(const std::string& api_key, const std::string& passphrase, const std::string& secret);

    static Request makeSubscribeAccountChannel();

    static Request makeSubscribePositionsChannel(const std::string& inst_type="ANY", const std::string& inst_id="");

    // 账户余额和持仓频道
    static Request makeSubscriBalanceAndPositionsChannel();

    // 订单频道
    // inst_type: SPOT：币币; MARGIN：币币杠杆 SWAP：永续合约 FUTURES：交割合约 OPTION：期权 ANY： 全部
    static Request makeSubscribeOrdersChannel(const std::string& inst_type="ANY", const std::string& inst_id="");

    static Request makeOrderReq(const std::string& inst_id, TradeMode trade_mode, const OrderData& order_data);

    static Request makeMultiOrderReq(const std::string& inst_id, TradeMode trade_mode, std::deque<OrderData>& orders);

    // 改单
    struct AmendInfo {
        std::string cliordid;
        std::string new_px;
        std::string new_sz;
    };
    static Request makeAmendOrderReq(const std::string& inst_id, std::deque<AmendInfo>& amend_data);

    // 撤单
    static Request makeCancelOrderReq(const std::string& inst_id, const std::string& cliordid, const std::string& ordid);
    static Request makeCancelMultiOrderReq(const std::string& inst_id, std::deque<std::string>& cliordids);


    // Public
    // 产品频道
    static Request makeSubscribeInstrumentsChannel(const std::string& inst_type);
    // 行情频道
    static Request makeSubscribeTickersChannel(const std::string& inst_id);
    // 交易频道
    static Request makeSubscribeTradesChannel(const std::string& inst_id);
    // 维护状态频道
    static Request makeSubscribeStatusChannel();

    // -1: require relogin, -2: require retry, 0: ok, 1: response
    static int parseReceivedData(const std::string& data, Response* out_resp);



};
