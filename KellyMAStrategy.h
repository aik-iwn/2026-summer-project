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
    double kellyFraction; // 凱利乘數(舉例:半凱利為0.5)

    double volumeMultiplier;       // 調節量的參數(保證空手時量已經突破)
    double cashBuffer;             // 現金緩衝比例(預設為0.98，防止明天開盤大漲跳空)
    double minTradeAmount;         // 最低交易金額門檻(過濾手續費低消)
    double whipSawBuffer;          // 季線緩衝帶參數
    double trailingStopPct;        // 移動停損比例(預設0.08，從最高點回檔8%就會自動停損)
    double highestPriceSinceEntry; // 紀錄持倉以來的最高價，用來判斷是否需停損
public:
    KellyMAStrategy(size_t shortPeriod = 10,
                    size_t longPeriod = 20,
                    size_t trendPeriod = 60,
                    size_t volumePeroid = 20,
                    double p = 0.65,
                    double b = 1.5,
                    double k_frac = 0.7,
                    double vol_mult = 1.05,
                    double buffer = 0.98,
                    double min_trade_amount = 15000.0,
                    double whipsaw_buffer = 0.01,
                    double trailing_stop_pct = 0.08)
        : shortMA(shortPeriod),
          longMA(longPeriod),
          trendMA(trendPeriod),
          volumeMA(volumePeroid),
          winRate(p),
          winLossRatio(b),
          kellyFraction(k_frac),
          volumeMultiplier(vol_mult),
          cashBuffer(buffer),
          minTradeAmount(min_trade_amount),
          whipSawBuffer(whipsaw_buffer),
          trailingStopPct(trailing_stop_pct) {};

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

        if (ac.getPosition() > 0) // 在有持倉時才需紀錄最高價，若是空手時便歸零
        {
            highestPriceSinceEntry = std::max(highestPriceSinceEntry, today.close);
        }
        else
        {
            highestPriceSinceEntry = 0.0;
        }
        double targetRatio = 0.0; // 目標股數占總資產比例
        // 短線方面是要多頭趨勢且站穩季線以上才會開始買入
        if (currShort > currLong && today.close > (currTrend * (1.0 + whipSawBuffer))) // 如果是空頭趨勢就空手(需大於季線才會做交易)
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
            if (ac.getPosition() == 0 && today.volume < currVolMA * volumeMultiplier) // 如果空手時沒有爆量，就不會進場
            {
                targetRatio = 0.0;
            }
        }
        if (ac.getPosition() && today.close < (highestPriceSinceEntry * (1 - trailingStopPct))) // 在任何情況下，如果回檔超過目前記錄以來最高價格的8%，就立刻清倉
        {
            targetRatio = 0.0;
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