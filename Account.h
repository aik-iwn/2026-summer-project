#pragma once

#include <string>
#include <vector>

struct Transaction
{
    std::string date;
    std::string type; //"BUY" or "SELL"
    double price;
    int shares;
    double fee;         // 手續費
    double tax;         // 稅金(賣出才有)
    double totalAmount; // price*shares+tax+fee
};

class Account
{
private:
    double balance;                    // 帳戶目前餘額
    int position;                      // 目前總持有股數
    double initialCapital;             // 初始總投入金額
    std::vector<Transaction> tradeLog; // 交易紀錄
public:
    Account(double initial_captial) : initialCapital(initial_captial), balance(initial_captial), position(0) {};
    bool buy(std::string date, double price, int share);
    bool sell(std::string date, double price, int share);
    double getInitialCapital() const { return initialCapital; };
    double getBalance() const; // 剩餘多少錢
    int getPosition() const;
    const std::vector<Transaction> &getTradeLog() const { return tradeLog; }; // 不會修改Account裡面變數，回傳時也無法修改
};