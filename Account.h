#pragma once

#include <string>
#include <vector>
#include <deque>

// 交易紀錄
struct Transaction
{
    std::string date;
    std::string type; //"BUY" or "SELL"
    double price;
    int shares;
    double fee;         // 手續費(買入賣出都有)
    double tax;         // 稅金(賣出才有)
    double totalAmount; // price*shares+tax+fee
};

// 目前持有的股票明細
struct StockDetail
{
    std::string date;
    double purchasePrice;
    int shares;       // 股票明細
    double totalCost; // 該次買入總成本
};

class Account
{
private:
    double balance;                    // 帳戶目前餘額
    double initialCapital;             // 初始總投入金額
    int position;                      // 目前總持有股數
    std::vector<Transaction> tradeLog; // 交易紀錄
    double total_holding_cost = 0;     // 目前總持有成本
    std::deque<StockDetail> inventory; // 股票持有明細管理倉
    int totalTrades = 0, winTrades = 0;

public:
    Account(double initial_captial) : initialCapital(initial_captial), balance(initial_captial), position(0) {};
    bool buy(std::string date, double price, int share);
    bool sell(std::string date, double price, int share);
    double getInitialCapital() const { return initialCapital; };
    double getBalance() const; // 剩餘多少錢
    int getPosition() const;
    int getTotalTrades() const { return totalTrades; };
    int getWinTrades() const { return winTrades; };
    const std::vector<Transaction> &getTradeLog() const { return tradeLog; }; // 不會修改Account裡面變數，回傳時也無法修改
};