#pragma once

#include "command.h"
#include "../utils/ws_session.h"
#include "../utils/concurrent_queue.h"
#include <thread>
#include <functional>
#include <deque>

namespace okex {


    class Channel {
    public:
        Channel(net::io_context& ioc, bool required_login,
            const std::string& host, const std::string& port, const std::string& path,
            const std::string socks_proxy = "");

        virtual ~Channel();

        typedef std::function<void(Command::Response&)> callback_func_t;

        void sendCmd(Command::Request&& req, callback_func_t callback);

        void reconnect();

        void waitConnected();

    private:
        void run();
        void cb_proc();
        void parseIncomeData(const std::string& data);

        virtual void onConnected() = 0;

    private:
        net::io_context& ioc_;
        const bool required_login_;

        std::string host_;
        std::string port_;
        std::string path_;
        std::string socks_proxy_;

        std::unique_ptr<std::thread> thread_;

        struct Cmd {
            Command::Request req;
            callback_func_t cb;
            enum class CmdType { Normal, Close } cmd_type = CmdType::Normal;
        };

        ConcurrentQueueT<Cmd> outq_;
        ConcurrentQueueT<std::string> inq_;
        std::deque<Cmd> waiting_resp_q_;

        std::unique_ptr<std::thread> cb_thread_;
        ConcurrentQueueT<std::function<void()> > cb_q_;

        std::mutex cond_mutex_;
        bool connected_ = false;
        std::condition_variable conn_condition_;
    };

}