#include <vector>
#include <iostream>
#include "BackTestEngine.h"
using namespace std;

BackTestEngine::BackTestEngine(double initial_capital, const vector<TradeData> &data) : my_ac(initial_capital), priceDataList(data) {};
void BackTestEngine::run()
{
    size_t total_days = priceDataList.size();
    for (size_t i = 0; i < total_days; i++)
    {
        const auto &current_data = priceDataList[i];
        if (i == 0)
        {
            bool test_buy = my_ac.buy(current_data.date, current_data.low, 10000);
        }
        if (i == (total_days - 1))
        {
            bool test_sell = my_ac.sell(current_data.date, current_data.low, 10000);
        }
    }
    cout << my_ac.getBalance() << " " << my_ac.getPosition() << "\n";
    const vector<Transaction> &trade = my_ac.getTradeLog();
    for (auto it = trade.begin(); it < trade.end(); it++)
    {
        cout << (*it).date << " " << (*it).type << " " << (*it).price << " " << (*it).shares << " " << (*it).fee << " " << (*it).tax << " " << (*it).totalAmount << "\n";
    }
}