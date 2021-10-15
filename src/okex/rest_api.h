#pragma once

#include "command.h"
#include "../utils/http_session.h"
#include "../utils/concurrent_queue.h"
#include <thread>
#include <functional>
#include <deque>

namespace okex {


    struct HistoryData {
        time_t time;
        float high, low, open, close;
    };

    class RestApi {
    public:
        RestApi(net::io_context& ioc,
            const std::string& host, const std::string& port,
            const std::string socks_proxy = "");

        virtual ~RestApi();

        typedef http::request<http::string_body> req_type;
        typedef http::response<http::string_body> resp_type;


        bool setLeverage(const std::string& inst_id, int lever, std::string* errmsg);
        bool checkOrderFilled(const std::string& inst_id);
        bool closePosition(const std::string& inst_id, OrderPosSide pos_side);

        // bar: [1m/3m/5m/15m/30m/1H/2H/4H/6H/12H/1D/1W/1M/3M/6M/1Y] 
        bool getMarkPriceHistoryCandles(const std::string& inst_id, const std::string& bar, std::deque<HistoryData>* out);

    private:
        bool sendCmd(const string& verbstr, const std::string& path, const std::string& reqdata, resp_type* resp);

    private:
        net::io_context& ioc_;

        std::string host_;
        std::string port_;
        std::string socks_proxy_;
    };


}