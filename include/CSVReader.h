#pragma once

#include <iostream> //使用cerr
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception> //用來抓錯誤
#include "TradeData.h"

class CSVReader
{
public:
    static std::vector<TradeData> readfile(const std::string &filename)
    {
        std::vector<TradeData> data;
        data.reserve(3000); // 預設記憶體大小，並非實際size大小
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return data;
        }
        std::string line;
        getline(file, line);
        while (getline(file, line))
        {
            std::stringstream ss(line); // 切割字串
            std::string token;
            TradeData datatem;
            if (getline(ss, token, ','))
            {
                datatem.date = token;
            }
            try
            {
                if (getline(ss, token, ','))
                {
                    datatem.open = std::stod(token);
                }
                if (getline(ss, token, ','))
                {
                    datatem.high = std::stod(token);
                }
                if (getline(ss, token, ','))
                {
                    datatem.low = std::stod(token);
                }
                if (getline(ss, token, ','))
                {
                    datatem.close = std::stod(token);
                }
                if (getline(ss, token, ','))
                {
                    if (!token.empty() && token.back() == '\r')
                    {
                        token.pop_back(); // 把每一行結尾的'r'字元給刪除
                    }
                    datatem.volume = std::stoll(token);
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "資料有誤，錯誤原因:" << e.what() << "\n";
                continue;
            }
            data.push_back(datatem);
        }
        file.close();
        return data;
    }
};