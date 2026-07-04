#pragma once

#include <vector>
#include "Account.h"
#include "TradeData.h"
using namespace std;

class BackTestEngine
{
private:
    Account my_ac;
    vector<TradeData> priceDataList;

public:
    BackTestEngine(double initial_capital, const vector<TradeData> &data);
    void run();
};
