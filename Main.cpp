#include <iostream>

#include "SparseSet.h"

void TestSparseSetInit();
void TestSparseSetEmplace();

int main() 
{
    TestSparseSetInit();
    TestSparseSetEmplace();

    return 0;
}

void TestSparseSetInit()
{
    //test
    Internal::SparseSet<int> set{ 20 };

    std::cout << "Size: " << set.Size() << "\n";
    std::cout << "Sparse Size: " << set.SparseSize() << "\n";
}

void TestSparseSetEmplace()
{
    Internal::SparseSet<int> set{ };
    set.Emplace(2, 0);
    //set.Emplace(2, 0); //assert triggered as expected

    std::cout << std::boolalpha << set.Contains(2) << "\n";
}
