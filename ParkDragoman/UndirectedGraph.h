#pragma once
#include <vector>
#include <numeric>
#include <unordered_set>
#include <algorithm>
//ElemType:元素类型
//WeightType:权值类型
template<class ElemType, class WeightType>
//无向图类
class UndirectedGraph
{
protected:
    //无穷大
    const WeightType infinity =
        std::numeric_limits<WeightType>::infinity();

    std::vector<std::vector<WeightType>> adjacencyMatrix;
public:
    //公开成员类型
    using elem_type = ElemType;
    using weight_type = WeightType;
    std::pair<std::vector<ElemType>, WeightType>
        GetShortestPath(ElemType from, ElemType to)const;
    std::vector<std::pair<std::vector<ElemType>, WeightType>>
        GetHamiltonianPath(ElemType from, ElemType to, int n)const;
    void Insert(ElemType from, ElemType to, WeightType weight);
    void Resize(ElemType size);
    UndirectedGraph() {}
    UndirectedGraph(ElemType size);
};

template<class ElemType, class WeightType>
inline std::pair<std::vector<ElemType>, WeightType>
UndirectedGraph<ElemType, WeightType>::
GetShortestPath(ElemType from, ElemType to) const
{
    //复制一个邻接矩阵
    auto a = this->adjacencyMatrix;
    //邻接矩阵大小
    auto size = a.size();
    //路径矩阵最初每行
    std::vector<ElemType> row(size);
    //生成从0到size-1的行
    std::iota(row.begin(), row.end(), 0);
    //构造路径矩阵
    std::vector<std::vector<ElemType>> p(size, row);
    //计数
    decltype(size) i, j, k;
    for (k = 0; k < size; ++k)
    {
        for (i = 0; i < size; ++i)
        {
            for (j = 0; j < size; ++j)
            {
                if (a[i][j] > a[i][k] + a[k][j])
                {
                    a[i][j] = a[i][k] + a[k][j];
                    p[i][j] = k;
                }
            }
        }
    }
    std::vector<ElemType> path;
    if (a[from][to] == this->infinity)
    {
        return{ path,a[from][to] };
    }
    path.push_back(from);
    while (p[path.back()][to] != to)
    {
        path.push_back(p[path.back()][to]);
    }
    path.push_back(to);
    return{ std::move(path),a[from][to] };
}

template<class ElemType, class WeightType>
inline std::vector<std::pair<std::vector<ElemType>, WeightType>>
UndirectedGraph<ElemType, WeightType>::
GetHamiltonianPath(ElemType from, ElemType to, int n) const
{
    if (from < 0 || from >= static_cast<ElemType>(this->adjacencyMatrix.size()))
    {
        throw std::runtime_error("Invaild Argument: from");
    }
    if (to < 0 || to >= static_cast<ElemType>(this->adjacencyMatrix.size()))
    {
        throw std::runtime_error("Invaild Argument: to");
    }
    if (from==to)
    {
        throw std::runtime_error("Invaild Arguments: from equals to");
    }
    if (n < 0)
    {
        throw std::runtime_error("Invaild Argument: n");
    }
    //存放访问记录
    std::unordered_set<ElemType> visited;
    //完成遍历
    bool completed = false;
    std::vector<ElemType> path;
    std::vector<std::pair<std::vector<ElemType>, WeightType>> result;
    auto GetWeightsSum = [this](std::vector<ElemType> path)
    {
        WeightType sum{0};
        for (auto it = path.cbegin() + 1;
            it != path.cend();
            ++it)
        {
            sum += this->adjacencyMatrix[*it][*(it - 1)];
        }
        return sum;
    };
    //结果没达到要求数目且未完成遍历
    while (static_cast<decltype(n)>(result.size()) < n
        && !completed)
    {
        visited.insert(from);
        path.push_back(from);
        for (auto i = 0; ; ++i)
        {
            //已经检查完所有相邻节点
            if (i == this->adjacencyMatrix.size())
            {
                //如果是检查完第一步
                if (path.size() == 1)
                {
                    //说明已经完成图的遍历
                    completed = true;
                    //结束检查
                    break;
                }
                //找到哈密尔顿路径
                if (path.size() == this->adjacencyMatrix.size())
                {
                    //求路径总长
                    auto sum = GetWeightsSum(path);
                    //加入结果
                    //使用vector的移动构造函数
                    //保证path.empty()为true
                    result.emplace_back(std::move(path), sum);
                    //结束检查
                    break;
                }
                //没有找到哈密尔顿路径
                //退一步
                i = path.back();
                path.pop_back();
                //擦掉访问记录
                visited.erase(i);
                //继续
                continue;
            }
            //没有到i的路径
            if (this->adjacencyMatrix[path.back()][i] == this->infinity
                //或者已经访问过i
                || visited.count(i) != 0)
            {
                continue;
            }
            //访问未访问过的相邻节点i
            path.push_back(i);
            //标记已访问i
            visited.insert(i);
        }
    }
    return result;
}

template<class ElemType, class WeightType>
inline void UndirectedGraph<ElemType, WeightType>::Insert(ElemType from, ElemType to, WeightType weight)
{
    //如果大小不够,扩展矩阵
    if (auto max = std::max(from, to) + 1;
        max > static_cast<ElemType>(this->adjacencyMatrix.size()))
    {
        //遍历
        for (auto it = this->adjacencyMatrix.begin();
            it != this->adjacencyMatrix.end();
            ++it)
        {
            it->resize(max, this->infinity);
        }
        //扩展行数
        this->adjacencyMatrix.resize(max,
            //使用了C++17的模板参数推导
            std::vector(max, this->infinity));
    }
    this->adjacencyMatrix[from][to] =
        this->adjacencyMatrix[to][from] = weight;
}

template<class ElemType, class WeightType>
inline void UndirectedGraph<ElemType, WeightType>::
Resize(ElemType size)
{
    //如果size范围错误
    if (size < 0)
    {
        //抛出异常
        throw std::runtime_error("Invaild Argument: size");
    }
    //循环遍历每一行
    for (auto it = this->adjacencyMatrix.begin();
        it != this->adjacencyMatrix.end();
        ++it)
    {
        //重新确定大小
        it->resize(size, this->infinity);
    }
    //重新确定行数
    this->adjacencyMatrix.resize(size, std::vector(size, this->infinity));
}

template<class ElemType, class WeightType>
inline UndirectedGraph<ElemType, WeightType>::
UndirectedGraph(ElemType size)
{
    //构造size*size的全为inf的矩阵
    this->adjacencyMatrix.resize(size, std::vector(size, this->infinity));
}
