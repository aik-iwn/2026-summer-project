#include <iostream>
#include <string>
#include "Account.h"

using namespace std;

bool Account::buy(string date, double price, int share)
{
    if (share <= 0)
    {
        cerr << "[" << date << "] 股數不能小於等於0，交易失敗\n";
        return false;
    }
    double stockcost = price * share;
    double comission = (int)(0.1425 * 0.01 * stockcost);
    if (comission < 20)
    {
        comission = 20; // 手續費小於20就以20來計算
    }
    double total = comission + stockcost;
    if (total > balance)
    {
        cerr << "[" << date << "] 餘額不足，交易失敗\n";
        return false;
    }
    balance -= total;
    position += share;
    total_holding_cost += total;
    Transaction t;
    t.date = date;
    t.type = "BUY";
    t.price = price;
    t.shares = share;
    t.fee = comission;
    t.tax = 0;
    t.totalAmount = total;
    t.realizedProfit = 0;
    tradeLog.push_back(t);
    StockDetail s;
    s.date = date;
    s.purchasePrice = price;
    s.shares = share;
    s.totalCost = total;
    inventory.push_back(s);
    return true;
}

bool Account::sell(string date, double price, int share)
{
    if (share <= 0)
    {
        cerr << "[" << date << "] 股數不能小於等於0，交易失敗\n";
        return false;
    }
    if (share > position)
    {
        cerr << "[" << date << "] 股數不足，交易失敗\n";
        return false;
    }
    int share_to_sell = share;      // 要賣的股數
    double total_realized_cost = 0; // 預計賣出所花費總成本
    while (share_to_sell > 0 && !inventory.empty())
    {
        auto &oldest = inventory.front();
        if (oldest.shares > share_to_sell)
        {
            double remove_cost = oldest.totalCost * share_to_sell / oldest.shares;
            total_realized_cost += remove_cost;
            total_holding_cost -= remove_cost;
            oldest.shares -= share_to_sell;
            oldest.totalCost -= remove_cost;
            share_to_sell = 0;
        }
        else
        {
            share_to_sell -= oldest.shares;
            total_realized_cost += oldest.totalCost;
            total_holding_cost -= oldest.totalCost;
            inventory.pop_front();
        }
    }
    double stockearn = price * share;
    double comission = (int)(0.1425 * 0.01 * stockearn);
    double tax = (int)(0.3 * 0.01 * stockearn);
    if (comission < 20)
    {
        comission = 20; // 手續費小於20就以20來計算
    }
    double total = stockearn - comission - tax;
    balance += total;
    position -= share;
    totalTrades++;
    if (total - total_realized_cost > 0)
    {
        winTrades++;
    }
    netProfit += (total - total_realized_cost);
    Transaction t;
    t.date = date;
    t.type = "SELL";
    t.price = price;
    t.shares = share;
    t.fee = comission;
    t.tax = tax;
    t.totalAmount = total;
    t.realizedProfit = (total - total_realized_cost);
    tradeLog.push_back(t);
    return true;
}