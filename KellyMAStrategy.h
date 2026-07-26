#pragma once

#include "Strategy.h"
#include "MovingAverage.h"
#include "Account.h"
#include <algorithm>
#include <cmath>

class KellyMAStrategy : public Strategy
{
private:
    MovingAverage shortMA; // 短均線
    MovingAverage longMA;  // 長均線
    double winRate;        // 凱利公式:預期勝率(p)
    double winLossRatio;   // 凱利公式:預期盈虧比(b)
    double kellyFraction;  // 凱利乘數(為使用半凱利，預設為0.5)
public:
    KellyMAStrategy(size_t shortPeriod = 10, size_t longPeriod = 50, double p = 0.55, double b = 1.5, double k_frac = 0.5)
        : shortMA(shortPeriod),
          longMA(longPeriod),
          winRate(p),
          winLossRatio(b),
          kellyFraction(k_frac) {};
    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        shortMA.addSample(today.close);
        longMA.addSample(today.close);
        if (!longMA.isReady())
        {
            return {Signal::HOLD, 0};
        }
        double currShort = shortMA.getValue();
        double currLong = longMA.getValue();
        double targetRatio = 0.0; // 目標股數占總資產比例
        if (currShort > currLong) // 如果是空頭趨勢就空手
        {
            double p = winRate;
            double q = 1.0 - p;
            double b = winLossRatio;
            double f = (b * p - q) / b;
            if (f > 0)
            {
                targetRatio = f * kellyFraction;
                targetRatio = std::min(targetRatio, 1.0); // 防止爆倉
            }
        }
        double current_stock_value = ac.getPosition() * today.close;
        double total_asset_value = ac.getBalance() + current_stock_value; // 目前總資產
        double target_stock_value = total_asset_value * targetRatio;
        int target_shares = static_cast<int>(target_stock_value / today.close); // 根據目標股數占總資產比例來決定目前持股多寡
        int current_shares = ac.getPosition();
        int diff = target_shares - current_shares; // 目標持股與現在持股數量差距
        Order order = {Signal::HOLD, 0};
        if (diff > 0)
        {
            order = {Signal::BUY, diff};
        }
        if (diff < 0)
        {
            order = {Signal::SELL, std::abs(diff)};
        }
        return order;
    }
};