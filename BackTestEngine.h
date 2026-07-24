#pragma once

#include <vector>
#include "Account.h"
#include "TradeData.h"
#include "Strategy.h"

class BackTestEngine
{
private:
    Account my_ac;
    std::vector<TradeData> priceDataList;
    Strategy *strategy;

public:
    BackTestEngine(double initial_capital, const std::vector<TradeData> &data, Strategy *strat);
    void run();
    double ROI();
};
// 在.h檔裡面不要使用using namspace std，避免命名域衝突，由於.h檔大家都會共用