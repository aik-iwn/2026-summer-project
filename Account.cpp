#include <iostream>
#include <string>
#include "Account.h"

using namespace std;

double Account::getBalance() const
{
    return balance;
}

int Account::getPosition() const
{
    return position;
}

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
    Transaction t;
    t.date = date;
    t.type = "BUY";
    t.price = price;
    t.shares = share;
    t.fee = comission;
    t.tax = 0;
    t.totalAmount = total;
    tradeLog.push_back(t);
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
    Transaction t;
    t.date = date;
    t.type = "SELL";
    t.price = price;
    t.shares = share;
    t.fee = comission;
    t.tax = tax;
    t.totalAmount = total;
    tradeLog.push_back(t);
    return true;
}