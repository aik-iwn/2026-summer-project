#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <exception> //用來抓錯誤
#include "TradeData.h"
using namespace std;

class CSVReader
{
public:
    static vector<TradeData> readfile(string filename)
    {
        vector<TradeData> data;
        data.reserve(3000); // 預設記憶體大小，並非實際size大小
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
                try
                {
                    datatem.open = stod(token);
                }
                catch (const exception &e)
                {
                    cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                    continue;
                }
            }
            if (getline(ss, token, ','))
            {
                try
                {
                    datatem.high = stod(token);
                }
                catch (const exception &e)
                {
                    cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                    continue;
                }
            }
            if (getline(ss, token, ','))
            {
                try
                {
                    datatem.low = stod(token);
                }
                catch (const exception &e)
                {
                    cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                    continue;
                }
            }
            if (getline(ss, token, ','))
            {
                try
                {
                    datatem.close = stod(token);
                }
                catch (const exception &e)
                {
                    cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                    continue;
                }
            }
            if (getline(ss, token, ','))
            {
                if (!token.empty() && token.back() == '\r')
                {
                    token.pop_back(); // 把每一行結尾的'r'字元給刪除
                }
                try
                {
                    datatem.volume = stoll(token);
                }
                catch (const exception &e)
                {
                    cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                    continue;
                }
            }
            data.push_back(datatem);
        }
        file.close();
        return data;
    }
};