#pragma once

#include "Strategy.h"
#include "MonotonicQueue.h"

class BreakoutStrategy : public Strategy
{
private:
    DonchianQueue donchianqueue;
    bool has_position;

public:
    BreakoutStrategy(int size = 20) : donchianqueue(size), has_position(false) {};
    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        double upper_band = donchianqueue.getUpperBand();
        double lower_band = donchianqueue.getLowerBand();
        Order order = {Signal::HOLD, 0};
        if (donchianqueue.isReady())
        {
            if (today.high > upper_band && !has_position)
            {
                int shares = ac.getBalance() * 0.99 / today.close;
                if (shares > 0)
                {
                    order.action = Signal::BUY;
                    order.shares = shares;
                    has_position = true;
                }
            }
            else if (today.low < lower_band && has_position)
            {
                order.action = Signal::SELL;
                order.shares = ac.getPosition();
                has_position = false;
            }
        }
        donchianqueue.push(today.high, today.low);
        return order;
    }
};