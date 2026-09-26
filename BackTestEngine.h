#pragma once

#include <vector>
#include <string>
#include "Account.h"
#include "TradeData.h"
#include "Strategy.h"

struct DailySnapShot
{
    std::string date;
    double closePrice;
    double cash;        // 剩餘現金
    int position;       // 持有股數
    double stockValue;  // 股票市值(股數*收盤價)
    double totalEquity; // 總資產(市值+現金)
    double drawDownPct; // 當天回徹率
};

class BackTestEngine
{
private:
    Account my_ac;
    std::vector<TradeData> priceDataList;
    Strategy *strategy;
    std::vector<DailySnapShot> dailyHistory; // 每日交易資訊(不管當日有沒有實質交易)

public:
    BackTestEngine(double initial_capital, const std::vector<TradeData> &data, Strategy *strat);
    void run();
    double ROI();
    void exportAllReport(const std::string &filename) const; // 交易流水帳+每日真實資產曲線
};
// 在.h檔裡面不要使用using namspace std，避免命名域衝突，由於.h檔大家都會共用