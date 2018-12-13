// ParkDragoman.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ParkDargoman.h"
#include <iostream>
#include <iterator>
#include <string>

int main()
{
    auto Print = [](const auto&path, auto distance)
    {
        std::cout << "路径: ";
        std::copy(path.cbegin(),
            path.cend() - 1,
            std::ostream_iterator<std::string>(std::cout, ","));
        std::cout << path.back()
            << "\n路程: " << distance << '\n';
    };
    try
    {
        ParkDargoman dargoman("data");
        for (;;)
        {
            try
            {
                std::cout << "\n1.最短路径\n"
                    << "2.游览路线\n"
                    << "3.退出\n";
                char choice;
                std::cin >> choice;
                switch (choice)
                {
                case '1':
                {
                    std::string from, to;
                    std::cout << "起点: ";
                    std::cin >> from;
                    std::cout << "终点: ";
                    std::cin >> to;
                    auto[path, distance] = dargoman.GetShortestPath(from, to);
                    Print(path, distance);
                }
                break;
                case '2':
                {
                    std::string from, to;
                    std::cout << "起点: ";
                    std::cin >> from;
                    std::cout << "终点: ";
                    std::cin >> to;
                    for (auto[path, distance] : dargoman.GetCircuitPath(from, to))
                    {
                        Print(path, distance);
                    }
                }
                break;
                case '3':
                    return 0;
                default:
                    break;
                }
            }
            catch (const std::exception& ex)
            {
                std::cout << ex.what() << '\n';
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << '\n';
    }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
