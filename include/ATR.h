#pragma once

#include <deque>
#include <algorithm>
#include <cmath>

class ATR
{
private:
    std::deque<double> tr_history;
    int win_size;
    double prev_close;
    double tr_sum;

public:
    ATR(int size = 20) : win_size(size), prev_close(-1.0), tr_sum(0.0) {};
    void push(double high, double low, double close)
    {
        double current_tr = high - low;
        if (prev_close > 0.0) // 以下為ATR參數比較
        {
            double gap_high = std::abs(high - prev_close);
            double gap_low = std::abs(low - prev_close);
            current_tr = std::max({current_tr, gap_high, gap_low}); // 超過兩個2的比較就要這樣寫
        }
        tr_history.push_back(current_tr);
        tr_sum += current_tr;
        if (tr_history.size() > win_size)
        {
            tr_sum -= tr_history.front();
            tr_history.pop_front();
        }
        prev_close = close;
    }
    double getATR() const
    {
        if (tr_history.empty())
        {
            return 0.0;
        }
        return tr_sum / tr_history.size();
    }
    bool isReady() const
    {
        return (int)tr_history.size() >= win_size;
    }
};