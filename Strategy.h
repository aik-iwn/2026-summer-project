#pragma once

#include "TradeData.h"
#include "Account.h"

enum class Signal
{
    BUY,
    SELL,
    HOLD
};

struct Order
{
    Signal action;
    int shares;
};

class Strategy
{
public:
    virtual ~Strategy() = default;
    virtual Order generateOrder(const TradeData &today, const Account &ac) = 0;
};