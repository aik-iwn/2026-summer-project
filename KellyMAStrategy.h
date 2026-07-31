#pragma once

#include "Strategy.h"
#include "MovingAverage.h"
#include "Account.h"
#include <algorithm>
#include <cmath>

class KellyMAStrategy : public Strategy
{
private:
    MovingAverage shortMA;  // 短均線
    MovingAverage longMA;   // 月均線
    MovingAverage trendMA;  // 季均線
    MovingAverage volumeMA; // 20日成交量均線

    double winRate;       // 凱利公式:預期勝率(p)
    double winLossRatio;  // 凱利公式:預期盈虧比(b)
    double kellyFraction; // 凱利乘數(為使用半凱利，預設為0.5)

    double cashBuffer;     // 現金緩衝比例(預設為0.98，防止明天開盤大漲跳空)
    double minTradeAmount; // 最低交易金額門檻(過濾手續費低消)
public:
    KellyMAStrategy(size_t shortPeriod = 5,
                    size_t longPeriod = 50,
                    size_t trendPeriod = 60,
                    size_t volumePeroid = 20,
                    double p = 0.55,
                    double b = 1.5,
                    double k_frac = 0.7,
                    double buffer = 0.98,
                    double min_trade_amount = 15000.0)
        : shortMA(shortPeriod),
          longMA(longPeriod),
          trendMA(trendPeriod),
          volumeMA(volumePeroid),
          winRate(p),
          winLossRatio(b),
          kellyFraction(k_frac),
          cashBuffer(buffer),
          minTradeAmount(min_trade_amount) {};
    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        shortMA.addSample(today.close);
        longMA.addSample(today.close);
        trendMA.addSample(today.close);
        volumeMA.addSample(today.volume);
        if (!trendMA.isReady())
        {
            return {Signal::HOLD, 0};
        }
        double currShort = shortMA.getValue();
        double currLong = longMA.getValue();
        double currTrend = trendMA.getValue();
        double currVolMA = volumeMA.getValue();

        double targetRatio = 0.0;                            // 目標股數占總資產比例
        if (currShort > currLong && today.close > currTrend) // 如果是空頭趨勢就空手(需大於季線才會做交易)
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
            if (ac.getPosition() == 0 && today.volume < currVolMA * 1.2) // 如果空手時沒有爆量，就不會進場
            {
                targetRatio = 0;
            }
        }
        double current_stock_value = ac.getPosition() * today.close;
        double total_asset_value = ac.getBalance() * cashBuffer + current_stock_value; // 目前總資產(預留2%安全資金)

        double target_stock_value = total_asset_value * targetRatio;
        int target_shares = static_cast<int>(target_stock_value / today.close); // 根據目標股數占總資產比例來決定目前持股多寡
        int current_shares = ac.getPosition();
        int diff = target_shares - current_shares; // 目標持股與現在持股數量差距
        Order order = {Signal::HOLD, 0};
        double trade_amount = std::abs(diff) * today.close;
        if (trade_amount >= minTradeAmount || target_shares == 0) // 大於最低賣出金額或全部清倉
        {
            if (diff > 0)
            {
                order = {Signal::BUY, diff};
            }
            if (diff < 0)
            {
                order = {Signal::SELL, std::abs(diff)};
            }
        }
        return order;
    }
};