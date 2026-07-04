#include <iostream>
#include <vector>
#include <chrono> //用來計算程式跑出結果所花的時間
#include "TradeData.h"
#include "CSVReader.h"
#include "Account.h"
#include "BackTestEngine.h"
using namespace std;

int main()
{
    // auto start = std::chrono::steady_clock::now();                  // 目前時間點
    vector<TradeData> dataset = CSVReader::readfile("../data.csv"); // 要記得寫"../data.csv"，這樣才會從前一層目錄開始找
    BackTestEngine engine(1500000, dataset);
    engine.run();
    // auto end = std::chrono::steady_clock::now();                    // 跑完資料的時間點
    // std::chrono::duration<double, std::milli> elapsed = end - start;
    // for (int i = 0; i < dataset.size(); i++)
    // {
    //     cout << dataset[i].date << " " << dataset[i].open << " " << dataset[i].high << " " << dataset[i].low << " " << dataset[i].close << " " << dataset[i].volume << "\n";
    // }
    // cout << "讀取耗時" << elapsed.count() << "ms\n";
    return 0;
}
