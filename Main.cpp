#include <iostream>

#include "SparseSet.h"

void TestSparseSetInit();
void TestSparseSetEmplace();
void TestSparseSetErase();
void TestSparseSetIteration();

int main() 
{
    TestSparseSetInit();
    TestSparseSetEmplace();
    TestSparseSetErase();
    TestSparseSetIteration();

    return 0;
}

void TestSparseSetInit()
{
    std::cout << "INIT \n";

    Internal::sparse_set<int> set{ 20 };

    std::cout << "Size: " << set.size() << "\n";
    std::cout << "Sparse Size: " << set.sparse_size() << "\n";
}

void TestSparseSetEmplace()
{
    std::cout << "\nEMPLACE \n";

    Internal::sparse_set<int> set{ };
    auto& element = set.emplace(2, 99);
    std::cout << "element: " << element << "\n";
    //set.Emplace(2, 0); //assert triggered as expected

    std::cout << std::boolalpha << set.contains(2) << "\n";

    //try emplacing the same element using the try emplace
    std::cout << std::boolalpha << set.try_emplace(2, 99).second << "\n";
    std::cout << std::dec << (*set.try_emplace(2, 99).first) << "\n";
}

void TestSparseSetErase()
{
    std::cout << "\nERASE \n";

    Internal::sparse_set<int> set{ };
    set.emplace(0, 100);
    set.emplace(1, 200);
    set.emplace(2, 300);

    std::cout << std::boolalpha << set.contains(0) << "\n";
    std::cout << std::boolalpha << set.contains(1) << "\n";
    std::cout << std::boolalpha << set.contains(2) << "\n";
    std::cout << "\n";

    set.erase(1);
    std::cout << std::boolalpha << set.contains(0) << "\n";
    std::cout << std::boolalpha << set.contains(1) << "\n";
    std::cout << std::boolalpha << set.contains(2) << "\n";
    std::cout << "\n";

    std::cout << std::boolalpha << set.remove(1) << "\n";
}

void TestSparseSetIteration()
{
    std::cout << "\nITERATION \n";

    Internal::sparse_set<int> set{ };
    set.emplace(0, 0);
    set.emplace(1, 1);
    set.emplace(2, 2);
    set.emplace(3, 3);

    for (auto&& e : set)
    {
        std::cout << e << "\n";
    }
}
