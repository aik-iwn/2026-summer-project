#pragma once

#include "ATR.h"
#include "Strategy.h"
#include "MonotonicQueue.h"
#include <algorithm>

class BreakoutStrategy : public Strategy
{
private:
    DonchianQueue donchianqueue; // 唐奇安通道
    ATR atr;                     // ATR指標協助計算買入多少量股票及停損線

    bool has_position;
    double peak_price;          // 持有股票時的最高價
    double trailing_stop_price; // 動態停損價位
    double risk_tolerance;      // 單筆所能容忍最大虧損比例
    double atr_multiplier;      // 停損容忍倍數
public:
    BreakoutStrategy(int size = 20, double risk_pct = 0.02, double multi = 3.0)
        : donchianqueue(size),
          atr(size),
          has_position(false),
          peak_price(0.0),
          trailing_stop_price(0.0),
          risk_tolerance(risk_pct),
          atr_multiplier(multi) {};
    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        double upper_band = donchianqueue.getUpperBand();
        double lower_band = donchianqueue.getLowerBand();
        double current_atr = atr.getATR();

        Order order = {Signal::HOLD, 0};
        if (donchianqueue.isReady() && atr.isReady())
        {
            if (has_position) // 在有持股期間才會更新止損點及賣出股票
            {
                if (today.high > peak_price) // 紀錄持股期間的最高價
                {
                    peak_price = today.high;
                }
                double new_stop_level = peak_price - (atr_multiplier * current_atr); // 重新計算止損線
                if (new_stop_level > trailing_stop_price)                            // 如果新止損點變高，那就更新
                {
                    trailing_stop_price = new_stop_level;
                }
                if (today.low < trailing_stop_price || today.low < lower_band)
                {
                    order.action = Signal::SELL;
                    order.shares = ac.getPosition();
                    has_position = false;
                    peak_price = 0.0;
                    trailing_stop_price = 0.0;
                }
            }
            else
            {
                if (today.high > upper_band) // 如果在未持股情況下且又突破就會買入
                {
                    double max_risk_amount = ac.getBalance() * risk_tolerance; // 允許最大虧損金額
                    double risk_per_share = current_atr * atr_multiplier;      // 每股將承擔的虧損風險
                    if (risk_per_share <= 0)
                    {
                        risk_per_share = 1.0;
                    }
                    int target_shares = max_risk_amount / risk_per_share;        // 依照風險反推回去算能買幾股
                    int max_affordable = (ac.getBalance() * 0.99) / today.close; // 所能花費最高金額
                    target_shares = std::min(target_shares, max_affordable);
                    if (target_shares > 0)
                    {
                        order.action = Signal::BUY;
                        order.shares = target_shares;
                        has_position = true;
                        peak_price = today.high;
                        trailing_stop_price = today.high - risk_per_share;
                    }
                }
            }
        }
        donchianqueue.push(today.high, today.low);
        atr.push(today.high, today.low, today.close);
        return order;
    }
};