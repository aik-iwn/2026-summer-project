#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include "TradeData.h"
using namespace std;

class CSVReader
{
public:
    static vector<TradeData> readfile(string filename)
    {
        vector<TradeData> data;
        data.reserve(2500); // 預設記憶體大小，並非實際size大小
        int i = 0;
        ifstream file(filename);
        if (!file.is_open())
        {
            return data;
        }
        string line;
        getline(file, line);
        while (getline(file, line))
        {
            stringstream ss(line); // 切割字串
            string token;
            TradeData datatem;
            if (getline(ss, token, ','))
            {
                datatem.date = token;
            }
            if (getline(ss, token, ','))
            {
                datatem.open = stod(token);
            }
            if (getline(ss, token, ','))
            {
                datatem.high = stod(token);
            }
            if (getline(ss, token, ','))
            {
                datatem.low = stod(token);
            }
            if (getline(ss, token, ','))
            {
                datatem.close = stod(token);
            }
            if (getline(ss, token, ','))
            {
                datatem.volume = stoll(token);
            }
            data.push_back(datatem);
        }
        file.close();
        return data;
    }
};