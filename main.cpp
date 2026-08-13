#include <iostream>
#include <vector>
#include <chrono> //用來計算程式跑出結果所花的時間
#include "TradeData.h"
#include "CSVReader.h"
#include "Account.h"
#include "BackTestEngine.h"
#include "KellyMAStrategy.h"
#include "DipBuyerStrategy.h"
#include "BreakoutStrategy.h"
using namespace std;

int main()
{
    auto start = std::chrono::steady_clock::now();                  // 目前時間點
    vector<TradeData> dataset = CSVReader::readfile("../data.csv"); // 要記得寫"../data.csv"，這樣才會從前一層目錄開始找

    DipBuyerStrategy dipbuyerstrategy;
    KellyMAStrategy kellyMAstrategy;
    BreakoutStrategy breakoutstrategy;

    // cout << "-----執行dipbuy交易策略-----\n";
    // BackTestEngine engine1(1500000, dataset, &dipbuyerstrategy);
    // engine1.run();
    // cout << "ROI:" << engine1.ROI() << "\n";
    // cout << "-----dipbuy交易策略完成-----\n\n";

    // cout << "-----執行KellyMAStrategy交易策略-----\n";
    // BackTestEngine engine2(1500000, dataset, &kellyMAstrategy);
    // engine2.run();
    // cout << "ROI:" << engine2.ROI() << "\n";
    // cout << "-----KellyMAStrategy交易策略完成-----\n\n";

    cout << "-----執行BreakoutStrategy交易策略-----\n";
    BackTestEngine engine3(1500000, dataset, &breakoutstrategy);
    engine3.run();
    cout << "ROI:" << engine3.ROI() << "\n";
    cout << "-----BreakoutStrategy交易策略完成-----\n\n";

    auto end = std::chrono::steady_clock::now(); // 跑完資料的時間點
    std::chrono::duration<double, std::milli> elapsed = end - start;
    cout << "讀取耗時" << elapsed.count() << "ms\n";
    return 0;
}