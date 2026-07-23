#pragma once

#include "TradeData.h"
#include "Account.h"

enum class Signal
{
    BUY,
    SELL,
    HOLD
};

class Strategy
{
public:
    virtual ~Strategy() = default;
    virtual Signal generateSignal(const TradeData &today, const Account &ac) = 0;
};