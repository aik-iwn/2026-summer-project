#pragma once

#include <vector>
#include "Account.h"
#include "TradeData.h"

class BackTestEngine
{
private:
    Account my_ac;
    std::vector<TradeData> priceDataList;

public:
    BackTestEngine(double initial_capital, const std::vector<TradeData> &data);
    void run();
};
// 在.h檔裡面不要使用using namspace std，避免命名域衝突，由於.h檔大家都會共用