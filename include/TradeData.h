#pragma once // 只能被定義一次(功能與#ifndef #define 類似)

#include <string>

struct TradeData
{
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long long volume;
};