#pragma once

#include <deque>
#include <utility>

class DonchianQueue
{
private:
    std::deque<std::pair<int, double>> max_q; // 編號;價格，設立一個價格由高至低排列的deque
    std::deque<std::pair<int, double>> min_q;
    int windowSize;
    int currentIndex;

public:
    DonchianQueue(int window_size) : windowSize(window_size), currentIndex(1) {};
    void push(double high_price, double low_price)
    {
        if (!max_q.empty() && max_q.front().first <= (currentIndex - windowSize))
        {
            max_q.pop_front();
        }
        while (!max_q.empty() && max_q.back().second <= high_price)
        {
            max_q.pop_back();
        }
        max_q.push_back({currentIndex, high_price});
        if (!min_q.empty() && min_q.front().first <= (currentIndex - windowSize))
        {
            min_q.pop_front();
        }
        while (!min_q.empty() && min_q.back().second >= low_price)
        {
            min_q.pop_back();
        }
        min_q.push_back({currentIndex, low_price});
        currentIndex++;
    }

    double getUpperBand() const
    {
        return max_q.empty() ? 0.0 : max_q.front().second;
    }
    double getLowerBand() const
    {
        return min_q.empty() ? 0.0 : min_q.front().second;
    }
    bool isReady() const
    {
        return currentIndex >= windowSize;
    }
};