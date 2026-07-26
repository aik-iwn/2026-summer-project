#pragma once

#include <deque>
#include <cstddef>

class MovingAverage
{
private:
    size_t windowSize; // 用size_t來確保windowSize是unsigned及容器相同
    std::deque<double> prices;
    double currentSum;

public:
    explicit MovingAverage(size_t period) : windowSize(period), currentSum(0.0) {}; // 用explicit可避免錯誤的隱性轉型
    void addSample(double price)
    {
        prices.push_back(price);
        currentSum += price;
        if (prices.size() > windowSize) // 如果更新資料完大小超過定義時間長度便刪除
        {
            currentSum -= prices.front();
            prices.pop_front();
        }
    }
    bool isReady() const
    {
        return prices.size() == windowSize;
    }
    double getValue() const
    {
        if (!isReady())
        {
            return 0.0;
        }
        return currentSum / static_cast<double>(windowSize); // 用static_cast來轉型較為精準
    }
};
// 均線產生器，可定義均線時間長度，可用來更新均線並計算目前均值