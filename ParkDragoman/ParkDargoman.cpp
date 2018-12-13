#include "pch.h"
#include "ParkDargoman.h"


ParkDargoman::ParkDargoman(const std::string & path)
{
    auto GetNonSpaceChar = [](auto&buffer)
    {
        while (std::isspace(buffer.peek()))
        {
            buffer.get();
        }
        return buffer.peek();
    };
    auto IndexOf = [](const std::string& str, char ch, int begin = 0)
    {
        auto size = static_cast<decltype(begin)>(str.size());
        for (auto i = begin; i < size; ++i)
        {
            if (str[i] == ch)
            {
                return i;
            }
        }
        return -1;
    };
    std::ifstream ifs(path);
    if (ifs.fail())
    {
        throw std::runtime_error("Open file failed!");
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();
    std::string line;
    std::vector<std::pair<std::string, int>> pathStrings;
    int lineCount = 0;
    while (buffer)
    {
        ++lineCount;
        switch (GetNonSpaceChar(buffer))
        {
        case 's':
            std::getline(buffer, line);
            if (line.size() <= 2 || line[1] != '=')
            {
                //抛出语法错误的异常
                throw std::runtime_error("Syntax error at line "
                    + std::to_string(lineCount));
            }
            {
                //读取s
                std::istringstream(line.substr(2)) >> this->s;
            }
            break;
        case '(':
            std::getline(buffer, line);
            if (line.back() != ')')
            {
                //抛出语法错误的异常
                throw std::runtime_error
                (
                    "Syntax error: unbalanced parenthesis, at line "
                    + std::to_string(lineCount)
                );
            }
            {
                //截取字符串,获取路径信息
                auto path = line.substr(1, line.size() - 2);
                //计数逗号,需要是两个
                if (std::count(path.cbegin(),
                    path.cend(),
                    ',') != 2)
                {
                    //抛出语法错误的异常
                    throw std::runtime_error
                    (
                        "Syntax error: wrong number of arguments, at line "
                        + std::to_string(lineCount)
                    );
                }
                pathStrings.emplace_back(std::move(path), lineCount);
            }
            break;
        case 'v':
            std::getline(buffer, line);
            if (line.size() <= 2 || line[1] != '=')
            {
                //抛出语法错误的异常
                throw std::runtime_error("Syntax error at line "
                    + std::to_string(lineCount));
            }
            {
                //去掉"v="
                line = line.substr(2);
                //分割字符串
                //起始位置
                int begin = 0,
                    //结束位置
                    next,
                    //地点的索引
                    index = -1;
                //找到下一个逗号位置
                while (next = IndexOf(line, ',', begin),
                    next != -1)
                {
                    //插入
                    this->tag2index.insert
                    (
                        {
                            //分割字符串
                            line.substr(begin,next - begin),
                            //索引号
                            ++index
                        }
                    );
                    //跳过逗号
                    begin = next + 1;
                }
                //插入最后一个位置
                this->tag2index.insert
                (
                    {
                        //分割剩下的字符串
                        line.substr(begin),
                        //索引号
                        ++index
                    }
                );
            }
            break;
        default:
            break;
        }
    }
    //生成索引到标签的映射
    //遍历标签到索引的映射
    for (auto it = this->tag2index.cbegin();
        it != this->tag2index.cend();
        ++it)
    {
        //交换元素插入
        this->index2tag.insert
        (
            {
                it->second,
                it->first
            }
        );
    }
    //设置地图大小
    this->map.Resize(this->tag2index.size());
    for (const auto&[path, lineCount] : pathStrings)
    {
        auto comma1 = IndexOf(path, ',');
        auto place1 = path.substr(0, comma1);
        auto it1 = this->tag2index.find(place1);
        if (it1 == this->tag2index.cend())
        {
            throw std::runtime_error("Place not found: " + place1
                + ", at line " + std::to_string(lineCount));
        }
        auto comma2 = IndexOf(path, ',', comma1 + 1);
        auto place2 = path.substr(comma1 + 1, comma2 - (comma1 + 1));
        auto it2 = this->tag2index.find(place2);
        if (it2 == this->tag2index.cend())
        {
            throw std::runtime_error("Place not found: " + place2
                + ", at line " + std::to_string(lineCount));
        }
        //距离
        decltype(this->map)::weight_type distance;
        //字符串转浮点到distance
        std::from_chars(path.data() + (comma2 + 1),
            path.data() + path.size(),
            distance);
        //插入到地图
        this->map.Insert(it1->second, it2->second, distance);
    }
}

std::vector<std::pair<std::vector<std::string>, float>> ParkDargoman::
GetCircuitPath(const std::string & from, const std::string & to) const
{
    auto it1 = this->tag2index.find(from);
    if (it1 == this->tag2index.cend())
    {
        throw std::runtime_error("Place not found: " + from);
    }
    auto it2 = this->tag2index.find(to);
    if (it2 == this->tag2index.cend())
    {
        throw std::runtime_error("Place not found: " + to);
    }
    if (it1->second == it2->second)
    {
        throw std::runtime_error("Starting point equals ending point");
    }
    std::vector<std::pair<std::vector<std::string>, float>> result;
    for (auto[path, distance] :
        this->map.GetHamiltonianPath(it1->second, it2->second, this->s))
    {
        std::vector<std::string> p;
        for (auto it = path.cbegin();
            it != path.cend();
            ++it)
        {
            p.emplace_back(this->index2tag.find(*it)->second);
        }
        result.emplace_back(std::move(p), distance);
    }
    if (result.empty())
    {
        throw std::runtime_error("Can't find a circuit path from \"" 
            + from + "\" to \"" + to + '\"');
    }
    return result;
}

std::pair<std::vector<std::string>, float> ParkDargoman::
GetShortestPath(const std::string & from, const std::string & to) const
{
    auto it1 = this->tag2index.find(from);
    if (it1 == this->tag2index.cend())
    {
        throw std::runtime_error("Place not found: " + from);
    }
    auto it2 = this->tag2index.find(to);
    if (it2 == this->tag2index.cend())
    {
        throw std::runtime_error("Place not found: " + to);
    }
    if (it1->second == it2->second)
    {
        return{ std::vector<std::string>() ,float(0) };
    }
    std::pair<std::vector<std::string>, float> result;
    auto[path, distance] =
        this->map.GetShortestPath(it1->second, it2->second);
    if (distance==std::numeric_limits<float>::infinity())
    {
        throw std::runtime_error("Can't reach \"" + to
            + "\" from \"" + from + '\"');
    }
    for (auto it = path.cbegin();
        it != path.cend();
        ++it)
    {
        result.first.emplace_back(this->index2tag.find(*it)->second);
    }
    result.second = distance;
    return result;
}
