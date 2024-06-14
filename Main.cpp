#include <iostream>

#include "SparseSet.h"

void TestSparseSetInit();
void TestSparseSetEmplace();
void TestSparseSetErase();
void TestSparseSetIteration();
void TestSparseSetRandomAccess();

int main() 
{
    TestSparseSetInit();
    TestSparseSetEmplace();
    TestSparseSetErase();
    TestSparseSetIteration();
    TestSparseSetRandomAccess();

    return 0;
}

void TestSparseSetInit()
{
    std::cout << "INIT \n";

    Internal::sparse_set<int> set{ 20 };

    std::cout << "Size: " << set.size() << "\n";
    std::cout << "Sparse Size: " << set.sparse_size() << "\n";
    std::cout << "Max Sparse Size: " << set.max_sparse_size() << "\n";

    std::cout << "\n";

    Internal::sparse_set<int> set2{ {1, 10}, {2, 20} };
    for (auto&& val : set2)
    {
        std::cout << val << "\n";
    }

    std::cout << "\n";

    //Internal::sparse_set<int> set3{};
    //set3 = set2;
    //set3 = std::move(set2);
    
    //auto set3{ set2 };
    auto set3{ std::move(set2) };
    
    for (auto&& val : set3)
    {
        std::cout << val << "\n";
    }
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
    std::cout << "\n";

    set.erase(0);
    set.erase(2);

    set.emplace(0, 100);
    set.emplace(1, 200);
    set.emplace(2, 300);

    for (auto&& val : set)
    {
        std::cout << val << "\n";
    }

    set.erase(set.begin());
    set.erase(set.cbegin() + 1);
    set.erase(set.begin());

    for (auto&& val : set)
    {
        std::cout << val << "\n";
    }

    std::cout << "\n";

    set.emplace(0, 100);
    set.emplace(1, 200);
    set.emplace(2, 300);
    set.emplace(3, 400);
    set.emplace(4, 500);

    auto it = set.erase(set.begin(), set.begin() + 1);
    std::cout << std::boolalpha << set.contains(0) << "\n";
    std::cout << std::boolalpha << set.contains(1) << "\n";
    std::cout << std::boolalpha << set.contains(2) << "\n";
    std::cout << std::boolalpha << set.contains(3) << "\n";
    std::cout << std::boolalpha << set.contains(4) << "\n";

    set.clear();
    set.shrink_to_fit();

    //std::cout << "\n\n\n";
    //set.emplace(0, 100);
    //set.emplace(1, 200);
    //set.emplace(2, 300);
    //set.emplace(3, 400);
    //set.emplace(4, 500);

    //for (auto&& val : set)
    //{
    //    std::cout << val << "\n";
    //}

    //std::cout << "\n";
}

void TestSparseSetIteration()
{
    std::cout << "\nITERATION \n";

    Internal::sparse_set<int> set{ };
    set.emplace(0, 0);
    set.emplace(1, 10);
    set.emplace(2, 20);
    set.emplace(3, 30);

    for (auto it{ set.begin()}; auto&& e : set)
    {
        std::cout << e;

        std::cout << ", " << set.sparse_index(it) << "\n";

        ++it;
    }

    std::cout << "\n";

    for (auto it = set.rbegin(); it != set.rend(); ++it)
    {
        std::cout << *it << ", " << set.sparse_index(it) << "\n";
    }
}

void TestSparseSetRandomAccess()
{
    std::cout << "\nRANDOM ACCESS\n";

    Internal::sparse_set<int> set{ };
    set.emplace(0, 0);
    set.emplace(1, 10);
    set.emplace(2, 20);
    set.emplace(3, 30);

    std::cout << set[2] << "\n";
    std::cout << set.get_or_emplace(2, 200) << "\n";
    std::cout << set.get_or_emplace(2) << "\n";
    std::cout << set.get_or_emplace(5) << "\n";
    std::cout << std::boolalpha << (set.find(5) != set.end()) << "\n";
    std::cout << set.at(5) << "\n";
    
    try
    {
        set.at(200);
    }
    catch (const Internal::sparse_set_out_of_range<Internal::sparse_set<int>::key_type>& e)
    {
        std::cerr << e.what() <<  " " << e.element() << "\n";
        set.emplace(e.element(), 2000);
    }

    std::cout << set.at(200) << "\n";
}