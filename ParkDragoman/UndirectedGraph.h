#pragma once
#include <vector>
#include <numeric>
#include <unordered_set>
#include <algorithm>
//ElemType:Ԫ������
//WeightType:Ȩֵ����
template<class ElemType, class WeightType>
//����ͼ��
class UndirectedGraph
{
protected:
    //�����
    const WeightType infinity =
        std::numeric_limits<WeightType>::infinity();

    std::vector<std::vector<WeightType>> adjacencyMatrix;
public:
    //������Ա����
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
    //����һ���ڽӾ���
    auto a = this->adjacencyMatrix;
    //�ڽӾ����С
    auto size = a.size();
    //·���������ÿ��
    std::vector<ElemType> row(size);
    //���ɴ�0��size-1����
    std::iota(row.begin(), row.end(), 0);
    //����·������
    std::vector<std::vector<ElemType>> p(size, row);
    //����
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
    //��ŷ��ʼ�¼
    std::unordered_set<ElemType> visited;
    //��ɱ���
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
    //���û�ﵽҪ����Ŀ��δ��ɱ���
    while (static_cast<decltype(n)>(result.size()) < n
        && !completed)
    {
        visited.insert(from);
        path.push_back(from);
        for (auto i = 0; ; ++i)
        {
            //�Ѿ�������������ڽڵ�
            if (i == this->adjacencyMatrix.size())
            {
                //����Ǽ�����һ��
                if (path.size() == 1)
                {
                    //˵���Ѿ����ͼ�ı���
                    completed = true;
                    //�������
                    break;
                }
                //�ҵ����ܶ���·��
                if (path.size() == this->adjacencyMatrix.size())
                {
                    //��·���ܳ�
                    auto sum = GetWeightsSum(path);
                    //������
                    //ʹ��vector���ƶ����캯��
                    //��֤path.empty()Ϊtrue
                    result.emplace_back(std::move(path), sum);
                    //�������
                    break;
                }
                //û���ҵ����ܶ���·��
                //��һ��
                i = path.back();
                path.pop_back();
                //�������ʼ�¼
                visited.erase(i);
                //����
                continue;
            }
            //û�е�i��·��
            if (this->adjacencyMatrix[path.back()][i] == this->infinity
                //�����Ѿ����ʹ�i
                || visited.count(i) != 0)
            {
                continue;
            }
            //����δ���ʹ������ڽڵ�i
            path.push_back(i);
            //����ѷ���i
            visited.insert(i);
        }
    }
    return result;
}

template<class ElemType, class WeightType>
inline void UndirectedGraph<ElemType, WeightType>::Insert(ElemType from, ElemType to, WeightType weight)
{
    //�����С����,��չ����
    if (auto max = std::max(from, to) + 1;
        max > static_cast<ElemType>(this->adjacencyMatrix.size()))
    {
        //����
        for (auto it = this->adjacencyMatrix.begin();
            it != this->adjacencyMatrix.end();
            ++it)
        {
            it->resize(max, this->infinity);
        }
        //��չ����
        this->adjacencyMatrix.resize(max,
            //ʹ����C++17��ģ������Ƶ�
            std::vector(max, this->infinity));
    }
    this->adjacencyMatrix[from][to] =
        this->adjacencyMatrix[to][from] = weight;
}

template<class ElemType, class WeightType>
inline void UndirectedGraph<ElemType, WeightType>::
Resize(ElemType size)
{
    //���size��Χ����
    if (size < 0)
    {
        //�׳��쳣
        throw std::runtime_error("Invaild Argument: size");
    }
    //ѭ������ÿһ��
    for (auto it = this->adjacencyMatrix.begin();
        it != this->adjacencyMatrix.end();
        ++it)
    {
        //����ȷ����С
        it->resize(size, this->infinity);
    }
    //����ȷ������
    this->adjacencyMatrix.resize(size, std::vector(size, this->infinity));
}

template<class ElemType, class WeightType>
inline UndirectedGraph<ElemType, WeightType>::
UndirectedGraph(ElemType size)
{
    //����size*size��ȫΪinf�ľ���
    this->adjacencyMatrix.resize(size, std::vector(size, this->infinity));
}
