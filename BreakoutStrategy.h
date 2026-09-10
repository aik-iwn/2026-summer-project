#pragma once

#include "ATR.h"
#include "Strategy.h"
#include "MonotonicQueue.h"
#include "MovingAverage.h"
#include <algorithm>

class BreakoutStrategy : public Strategy
{
private:
    DonchianQueue entry_dq; // 唐奇安通道(20日進場)
    DonchianQueue exit_dq;  // 唐奇安通道(20日出場)
    ATR atr;                // ATR指標協助計算買入多少量股票及停損線
    MovingAverage MA10;     // 10日短均線

    bool has_position;
    double peak_price;          // 持有股票時的最高價
    double trailing_stop_price; // 動態停損價位
    double risk_tolerance;      // 單筆所能容忍最大虧損比例
    double atr_multiplier;      // 停損容忍倍數
public:
    BreakoutStrategy(int entry_size = 20, int exit_size = 20, double risk_pct = 1, double multi = 7)
        : entry_dq(entry_size),
          exit_dq(exit_size),
          atr(14),
          MA10(10),
          has_position(false),
          peak_price(0.0),
          trailing_stop_price(0.0),
          risk_tolerance(risk_pct),
          atr_multiplier(multi) {};

    Order generateOrder(const TradeData &today, const Account &ac) override
    {
        double upper_band = entry_dq.getUpperBand();
        double lower_band = exit_dq.getLowerBand();
        double current_atr = atr.getATR();

        MA10.addSample(today.close);
        bool is_bull_market = (today.close > MA10.getValue());
        Order order = {Signal::HOLD, 0};

        if (MA10.isReady())
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
                if (today.low < lower_band && today.close < trailing_stop_price)
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
                if (today.high > upper_band && is_bull_market) // 如果在未持股情況下且又突破就會買入(要在多頭市場200MA之上)
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
        entry_dq.push(today.high, today.low);
        exit_dq.push(today.high, today.low);
        atr.push(today.high, today.low, today.close);
        return order;
    }
};
/*
本策略由三個指標共同協助完成，分別為唐奇安通道(進出場時機判斷)、ATR(計算買入多少量及停損線)、
10MA(短均線用來判斷牛市，均線太長反而會流失掉好的買賣時機)。賣出方式為一次全部賣出，買入則是計
算在此風險情況下應買入多少量(參照程式碼計算方式)，而上述所有控制參數為大量測試下所得出的最佳結
果，可以依照個人對於風險及獲益要求來做些微調整，。
*/