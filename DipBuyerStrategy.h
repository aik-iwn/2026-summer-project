#pragma once

#include "Strategy.h"

class DipBuyerStrategy : public Strategy
{
private:
    double last_close = -1;

public:
    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        Order order;
        if (last_close < 0)
        {
            last_close = today.close;
            order.action = Signal::HOLD;
            order.shares = 0;
            return order;
        }
        if (today.close < last_close)
        {
            order.action = Signal::BUY;
            order.shares = 100;
        }
        else if (today.close > last_close && ac.getPosition() > 0)
        {
            order.action = Signal::SELL;
            order.shares = ac.getPosition();
        }
        last_close = today.close;
        return order;
    }
};