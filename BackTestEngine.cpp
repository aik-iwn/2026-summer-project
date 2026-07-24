#include <iostream>
#include <vector>
#include <algorithm>
#include "Strategy.h"
#include "BackTestEngine.h"
using namespace std;

BackTestEngine::BackTestEngine(double initial_capital, const std::vector<TradeData> &data, Strategy *strat) : my_ac(initial_capital), priceDataList(data), strategy(strat) {};

double BackTestEngine::ROI()
{
    double finalAsset = my_ac.getBalance() + my_ac.getPosition() * priceDataList.back().close;
    double roi = (finalAsset - my_ac.getInitialCapital()) / my_ac.getInitialCapital() * 100;
    return roi;
}

void BackTestEngine::run()
{
    double peakEquity = my_ac.getInitialCapital(), max_drawDown = 0; // 用來計算最大回撤(Max drawdown)
    for (const auto &today : priceDataList)
    {
        Signal signal = strategy->generateSignal(today, my_ac);
        if (signal == Signal::BUY)
        {
            my_ac.buy(today.date, today.close, 100);
        }
        else if (signal == Signal::SELL)
        {
            my_ac.sell(today.date, today.close, my_ac.getPosition());
        }
        double currentEquity = my_ac.getBalance() + my_ac.getPosition() * today.close;
        peakEquity = max(peakEquity, currentEquity);
        max_drawDown = max(max_drawDown, (peakEquity - currentEquity) / peakEquity);
    }
    const auto &trade = my_ac.getTradeLog();
    for (const auto &t : trade)
    {
        cout << t.date << " " << t.type << " " << t.price << " " << t.shares << " " << t.fee << " " << t.tax << " " << t.totalAmount << " " << t.realizedProfit << "\n";
    }
    cout << "剩餘金額:" << my_ac.getBalance() << "\n";
    cout << "剩餘股數:" << my_ac.getPosition() << "\n";
    cout << "淨利所得:" << my_ac.getNetProfit() << "\n";
    cout << "最大回撤:" << max_drawDown * 100 << "%\n";
    cout << "總賣出次數:" << my_ac.getTotalTrades() << "\n";
    cout << "總獲利次數:" << my_ac.getWinTrades() << "\n";
    cout << "總虧損次數:" << my_ac.getTotalTrades() - my_ac.getWinTrades() << "\n";
}