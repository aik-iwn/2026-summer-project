#include <iostream>
#include <vector>
#include <string>
#include <chrono> //用來計算程式跑出結果所花的時間
#include "TradeData.h"
#include "CSVReader.h"
#include "Account.h"
#include "Strategy.h"
#include "BackTestEngine.h"
#include "KellyMAStrategy.h"
#include "DipBuyerStrategy.h"
#include "BreakoutStrategy.h"

using namespace std;

void executeBackTest(const string &strategyName, Strategy *strategy, const vector<TradeData> &dataset, double capital = 1500000)
{
    cout << "-----執行" << strategyName << "交易策略-----\n";
    BackTestEngine engine(capital, dataset, strategy);
    engine.run();
    cout << "ROI:" << engine.ROI() << "\n";
    cout << "-----" << strategyName << "交易策略完成-----\n";
}

int main()
{
    cout << "正在載入交易資料...\n";
    vector<TradeData> dataset = CSVReader::readfile("../data.csv"); // 要記得寫"../data.csv"，這樣才會從前一層目錄開始找
    if (dataset.empty())
    {
        cout << "載入失敗，請確認../data.csv是否存在!\n";
    }
    cout << "資料載入完成，共" << dataset.size() - 1 << "個交易日。\n\n";

    DipBuyerStrategy dipbuyer;
    KellyMAStrategy kellyMA;
    BreakoutStrategy breakout;

    cout << "========================================\n";
    cout << "   C++ 高頻量化回測系統 - 策略啟動介面  \n";
    cout << "========================================\n";
    cout << "[1] DipBuyerStrategy\n";
    cout << "[2] KellyMAStrategy  \n";
    cout << "[3] BreakoutStrategy\n";
    cout << "[4] 策略對決 (一口氣跑完 3 組評比)\n";
    cout << "[5] 再次顯示介面選單\n";
    cout << "[0] 離開系統\n";

    int choice = 0;
    while (true)
    {
        cout << "請輸入選項 (0-5):";
        cin >> choice;
        cout << "\n";
        if (choice == 0)
        {
            cout << "系統安全退出。\n";
            break;
        }
        std::chrono::time_point<std::chrono::steady_clock> start, end;
        switch (choice)
        {
        case 1:
            start = std::chrono::steady_clock::now(); // 目前時間點
            executeBackTest("DipBuyerStrategy", &dipbuyer, dataset);
            end = std::chrono::steady_clock::now(); // 跑完資料的時間點
            break;
        case 2:
            start = std::chrono::steady_clock::now(); // 目前時間點
            executeBackTest("KellyMAStrategy", &kellyMA, dataset);
            end = std::chrono::steady_clock::now(); // 跑完資料的時間點
            break;
        case 3:
            start = std::chrono::steady_clock::now(); // 目前時間點
            executeBackTest("BreakoutStrategy", &breakout, dataset);
            end = std::chrono::steady_clock::now(); // 跑完資料的時間點
            break;
        case 4:
            start = std::chrono::steady_clock::now(); // 目前時間點
            executeBackTest("DipBuyerStrategy", &dipbuyer, dataset);
            cout << "\n";
            executeBackTest("KellyMAStrategy", &kellyMA, dataset);
            cout << "\n";
            executeBackTest("BreakoutStrategy", &breakout, dataset);
            end = std::chrono::steady_clock::now(); // 跑完資料的時間點
            break;
        case 5:
            cout << "[1] DipBuyerStrategy\n";
            cout << "[2] KellyMAStrategy  \n";
            cout << "[3] BreakoutStrategy\n";
            cout << "[4] 策略對決 (一口氣跑完 3 組評比)\n";
            cout << "[5] 再次顯示介面選單\n";
            cout << "[0] 離開系統\n\n";
            break;
        default:
            cout << "輸入錯誤，請重新輸入\n";
            break;
        }
        std::chrono::duration<double, std::milli> elapsed = end - start;
        if (elapsed.count() != 0)
        {
            cout << "執行耗時" << elapsed.count() << "ms\n\n";
        }
    }
    return 0;
}