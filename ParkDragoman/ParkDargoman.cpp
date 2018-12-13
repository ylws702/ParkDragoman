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
                //�׳��﷨������쳣
                throw std::runtime_error("Syntax error at line "
                    + std::to_string(lineCount));
            }
            {
                //��ȡs
                std::istringstream(line.substr(2)) >> this->s;
            }
            break;
        case '(':
            std::getline(buffer, line);
            if (line.back() != ')')
            {
                //�׳��﷨������쳣
                throw std::runtime_error
                (
                    "Syntax error: unbalanced parenthesis, at line "
                    + std::to_string(lineCount)
                );
            }
            {
                //��ȡ�ַ���,��ȡ·����Ϣ
                auto path = line.substr(1, line.size() - 2);
                //��������,��Ҫ������
                if (std::count(path.cbegin(),
                    path.cend(),
                    ',') != 2)
                {
                    //�׳��﷨������쳣
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
                //�׳��﷨������쳣
                throw std::runtime_error("Syntax error at line "
                    + std::to_string(lineCount));
            }
            {
                //ȥ��"v="
                line = line.substr(2);
                //�ָ��ַ���
                //��ʼλ��
                int begin = 0,
                    //����λ��
                    next,
                    //�ص������
                    index = -1;
                //�ҵ���һ������λ��
                while (next = IndexOf(line, ',', begin),
                    next != -1)
                {
                    //����
                    this->tag2index.insert
                    (
                        {
                            //�ָ��ַ���
                            line.substr(begin,next - begin),
                            //������
                            ++index
                        }
                    );
                    //��������
                    begin = next + 1;
                }
                //�������һ��λ��
                this->tag2index.insert
                (
                    {
                        //�ָ�ʣ�µ��ַ���
                        line.substr(begin),
                        //������
                        ++index
                    }
                );
            }
            break;
        default:
            break;
        }
    }
    //������������ǩ��ӳ��
    //������ǩ��������ӳ��
    for (auto it = this->tag2index.cbegin();
        it != this->tag2index.cend();
        ++it)
    {
        //����Ԫ�ز���
        this->index2tag.insert
        (
            {
                it->second,
                it->first
            }
        );
    }
    //���õ�ͼ��С
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
        //����
        decltype(this->map)::weight_type distance;
        //�ַ���ת���㵽distance
        std::from_chars(path.data() + (comma2 + 1),
            path.data() + path.size(),
            distance);
        //���뵽��ͼ
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
