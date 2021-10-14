#ifndef BASE_TYPE_H_
#define BASE_TYPE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>


using std::string;

#define countof(arg) (sizeof(arg) / sizeof(arg[0]))

#define ASSERT_UNREACHABLE() assert("unreachable code" == 0)

#define UNUSED(expr) do { (void)(expr); } while (0)

#define DISABLE_COPY(classtype) \
    classtype(const classtype&) = delete; \
    classtype& operator=(const classtype&) = delete;


template<typename CallableT>
struct ScopeExit final {
    explicit ScopeExit(CallableT&& callable)
        : callable_(std::forward<CallableT>(callable)) {
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) = default;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit& operator=(ScopeExit&&) = default;

    ~ScopeExit() {
        if (!cancelled_) {
            callable_();
        }
    }

    void cancel() {
        cancelled_ = true;
    }

private:
    CallableT callable_;
    bool cancelled_{ false };
};

template<typename CallableT>
ScopeExit<CallableT>
make_scope_exit(CallableT && callable) {
    return ScopeExit<CallableT>(std::forward<CallableT>(callable));
}


// 交易下单
enum class OrderSide { Buy, Sell };

enum class  OrderPosSide { Net, Long, Short };

enum class OrderType {
    Market, // 市价单
    Limit // 限价单
};

enum class TradeMode {
    Isolated, //逐仓
    Cross,  // 全仓
    Cash //现金
};

struct OrderData {
    std::string clordid;
    OrderSide side;
    OrderPosSide pos_side;
    std::string px;
    std::string amount;
    OrderType order_type = OrderType::Limit;
};

enum class OrderStatus {
    Empty,
    Canceled,
    Live,
    PartiallyFilled,
    Filled,
    Failure,
};


inline std::string toString(OrderStatus status) {
    switch (status) {
    case OrderStatus::Empty:
        return "empty";
    case OrderStatus::Canceled:
        return "canceled";
    case OrderStatus::Live:
        return "live";
    case OrderStatus::PartiallyFilled:
        return "partfilled";
    case OrderStatus::Filled:
        return "filled";
    default:
        return "";
    }
}

#endif
