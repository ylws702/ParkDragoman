#pragma once

#include "UndirectedGraph.h"
#include <execution>
#include <string>
#include <cctype>
#include <vector>
#include <fstream>
#include <sstream>
#include <charconv>
#include <unordered_map>
class ParkDargoman
{
protected:
    UndirectedGraph<int, float> map;
    std::unordered_map<std::string, int> tag2index;
    std::unordered_map< int, std::string> index2tag;
    //最多显示的路径条数
    int s;
public:
    ParkDargoman(const std::string& path);
    std::vector<std::pair<std::vector<std::string>, float>>
        GetCircuitPath(const std::string& from, const std::string& to)const;
    std::pair<std::vector<std::string>, float>
        GetShortestPath(const std::string& from, const std::string& to)const;
};

