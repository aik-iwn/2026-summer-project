#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Strategy.h"
#include "DipBuyerStrategy.h"
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
        Order orders = strategy->generateOrder(today, my_ac);
        if (orders.action == Signal::BUY)
        {
            my_ac.buy(today.date, today.close, orders.shares);
        }
        else if (orders.action == Signal::SELL)
        {
            my_ac.sell(today.date, today.close, orders.shares);
        }
        double stackValue = my_ac.getPosition() * today.close;
        double currentEquity = my_ac.getBalance() + stackValue;
        peakEquity = max(peakEquity, currentEquity);
        double ddpct = (peakEquity - currentEquity) / peakEquity * 100;
        max_drawDown = max(max_drawDown, ddpct);
        dailyHistory.push_back({today.date,
                                today.close,
                                my_ac.getBalance(),
                                my_ac.getPosition(),
                                stackValue,
                                currentEquity,
                                ddpct});
    }
    const auto &trade = my_ac.getTradeLog(); // 交易明細
    cout << "剩餘金額:" << my_ac.getBalance() << "\n";
    cout << "剩餘股數:" << my_ac.getPosition() << "\n";
    cout << "淨利所得:" << my_ac.getNetProfit() << "\n";
    cout << "目前總資產:" << my_ac.getBalance() + my_ac.getPosition() * priceDataList.back().close << "\n";
    cout << "最大回撤:" << max_drawDown << "%\n";
    cout << "總賣出次數:" << my_ac.getTotalTrades() << "\n";
    cout << "總獲利次數:" << my_ac.getWinTrades() << "\n";
    cout << "總虧損次數:" << my_ac.getTotalTrades() - my_ac.getWinTrades() << "\n";
}

void BackTestEngine::exportAllReport(const string &filename) const
{
    ofstream outTradeFile("../trades/" + filename + "_trade.csv"); // 輸出檔
    ofstream outEquityFile("../equity/" + filename + "_equity.csv");
    if (!outTradeFile.is_open())
    {
        cerr << "無法成功建立報表" << filename << "\n";
        return;
    }
    if (!outEquityFile.is_open())
    {
        cerr << "無法成功建立報表" << filename << "\n";
        return;
    }
    outTradeFile << "Date,Type,Price,Shares,Fee,Tax,TotalAmount,RealizedProfit\n";
    const auto &trade = my_ac.getTradeLog();
    for (const auto &t : trade)
    {
        outTradeFile << t.date << ','
                     << t.type << ','
                     << t.price << ','
                     << t.shares << ','
                     << t.fee << ','
                     << t.tax << ','
                     << t.totalAmount << ','
                     << t.realizedProfit << '\n';
    }
    outTradeFile.close(); // 要關閉才會將緩衝區資料輸入至file
    cout << ">>>交易紀錄報表已成功彙至: " << "../trades/" << filename << "_trade.csv" << "\n";

    outEquityFile << "Date,Close,Cash,Position,StockValue,TotalEquity,DrawdownPct\n";
    for (const auto &d : dailyHistory)
    {
        outEquityFile << d.date << ","
                      << d.closePrice << ","
                      << d.cash << ","
                      << d.position << ","
                      << d.stockValue << ","
                      << d.totalEquity << ","
                      << d.drawDownPct << "\n";
    }
    outEquityFile.close();
    cout << ">>>每日真實資產曲線報表已匯出至: " << "../equity/" << filename << "_equity.csv" << "\n\n";
}