#include <iostream>
#include <vector>
#include "TradeData.h"
#include "CSVReader.h"
using namespace std;

int main()
{
    vector<TradeData> dataset = CSVReader::readfile("../data.csv"); // 要記得寫"../data.csv"，這樣才會從前一層目錄開始找
    for (int i = 0; i < dataset.size(); i++)
    {
        cout << dataset[i].date << " " << dataset[i].open << " " << dataset[i].high << " " << dataset[i].low << " " << dataset[i].close << " " << dataset[i].volume << "\n";
    }
    return 0;
}