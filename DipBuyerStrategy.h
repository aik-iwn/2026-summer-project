#pragma once

#include "Strategy.h"

class DipBuyerStrategy : public Strategy
{
private:
    double last_close = -1;

public:
    Signal generateSignal(const TradeData &today, const Account &ac) override
    {
        if (last_close < 0)
        {
            last_close = today.close;
            return Signal ::HOLD;
        }
        Signal sig = Signal::HOLD;
        if (today.close < last_close)
        {
            sig = Signal::BUY;
        }
        else if (today.close > last_close && ac.getPosition() > 0)
        {
            sig = Signal::SELL;
        }
        last_close = today.close;
        return sig;
    }
};