#include <iostream>
#include <string>

#include <chrono>

#include <random>

#include "SparseSet.h"

void TestSparseSetInit();
void TestSparseSetEmplace();
void TestSparseSetErase();
void TestSparseSetIteration();
void TestSparseSetRandomAccess();

void TestComplexTypes();

void TestSorting();

int RandomInt(int min, int max) 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}


int main() 
{
    TestSparseSetInit();
    TestSparseSetEmplace();
    TestSparseSetErase();
    TestSparseSetIteration();
    TestSparseSetRandomAccess();

    TestComplexTypes();

    TestSorting();

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


    std::cout << "\n";

    //set.swap_elements(0, 3);

    for (auto it{ set.begin() }; auto && e : set)
    {
        std::cout << e;

        std::cout << ", " << set.sparse_index(it) << "\n";

        ++it;
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

void TestComplexTypes()
{
    std::cout << "\nCOMPLEX TYPES\n";

    //Type that will use the trivially copyable constexpr version of erase,...
    struct ComplexType1 final
    {
        int integerVal;
        bool boolVal = false;
    };

    static_assert(std::is_trivially_copyable_v<ComplexType1>, "type 1 is not trivially copyable");

    Internal::sparse_set<ComplexType1> set1;
    set1.emplace(2, 200);
    set1.emplace(3, 300);
    set1.emplace(5, 500);

    for (auto&& e : set1)
    {
        std::cout << e.integerVal << "\n";
    }

    set1.erase(3);
    
    std::cout << "\n";

    for (auto&& e : set1)
    {
        std::cout << e.integerVal << "\n";
    }

    std::cout << "\n";

    struct ComplexType2 final
    {
        const int integerVal; //ensure its not copy && move assignable by making const
        std::string strVal = " "; //ensure it is not trivially copyable
    };

    static_assert(!std::is_trivially_copyable_v<ComplexType2> && 
                  !std::is_nothrow_move_assignable_v<ComplexType2> && 
                  !std::is_nothrow_copy_assignable_v<ComplexType2>, "non assignable type");

    Internal::sparse_set<ComplexType2> set2;
    set2.emplace(1, 100);
    set2.emplace(9, 900);
    set2.emplace(28, 2800); 

    for (auto it{set2.begin()}; auto && e : set2)
    {
        std::cout << e.integerVal << ", ";
        std::cout << set2.sparse_index(it) << "\n";
        ++it;
    }
    std::cout << "\n";

    set2.erase(9);

    for (auto it{ set2.begin() }; auto && e : set2)
    {
        std::cout << e.integerVal << ", ";
        std::cout << set2.sparse_index(it) << "\n";
        ++it;
    }
    std::cout << "\n";

   // set2.try_swap_values(1, 28);
    set2.try_swap_elements(1, 28);

    for (auto it{ set2.begin() }; auto && e : set2)
    {
        std::cout << e.integerVal << ", ";
        std::cout << set2.sparse_index(it) << "\n";
        ++it;
    }
    std::cout << "\n";


    struct ComplexType3 final
    {
        int integerVal;
        std::string strVal = " "; //ensure it is not trivially copyable
    };
    static_assert(!std::is_trivially_copyable_v<ComplexType3> &&
        std::is_nothrow_move_assignable_v<ComplexType3>, "assignable type");

    Internal::sparse_set<ComplexType3> set3;
    set3.emplace(1, 100);
    set3.emplace(9, 900);
    set3.emplace(28, 2800);

    for (auto it{ set3.begin() }; auto && e : set3)
    {
        std::cout << e.integerVal << ", ";
        std::cout << set3.sparse_index(it) << "\n";
        ++it;
    }
    std::cout << "\n";

    set3.erase(set3.end() - 1);
    //set3.swap_elements(set3.begin(), set3.end() - 1);

    for (auto it{ set3.begin() }; auto && e : set3)
    {
        std::cout << e.integerVal << ", ";
        std::cout << set3.sparse_index(it) << "\n";
        ++it;
    }
}

void TestSorting() 
{
    std::cout << "\nSORTING\n";

    Internal::sparse_set<int> set1{};

    constexpr int NUM_ELEMENTS{ 100 };
    constexpr int NUM_TRIALS{ 1 };

    std::vector<long long> durations1{};

    for (int trial{ 0 }; trial < NUM_TRIALS; ++trial) 
    {
        set1.clear();

        for (int i = 0; i < NUM_ELEMENTS; ++i)
        {
            const int randomValue = RandomInt(0, 1000000);
            const int randomIdx = RandomInt(i, i * 100);

            set1.try_emplace(randomIdx, randomValue);
        }

        auto start1 = std::chrono::high_resolution_clock::now();
        
        //set1.sort();
        set1.sort([](const auto& a, const auto& b)
            {
                return a < b;
            });

        auto end1 = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - start1);
        durations1.emplace_back(duration1.count());

        std::cout << "Sorted set 1:\n";
        for (auto it = set1.begin(); it != set1.end(); ++it)
        {
            std::cout << *it << ", ";
            std::cout << set1.sparse_index(it) << "\n";
        }

        std::cout << " \n\n";
        std::cout << std::boolalpha << set1.is_sorted() << "\n";
    }

    auto calc_avg = 
        [](std::vector<long long>& durations) -> long long
        {
            std::sort(durations.begin(), durations.end());

            const int discardCount { static_cast<int>(durations.size() * 0.1f)};
            const auto sum{ std::accumulate(durations.begin() + discardCount, durations.end() - discardCount, 0LL) };

            return sum / durations.size();
        };

    const auto avgDuration1{ calc_avg(durations1) };

    std::cout << "\n\n";
    std::cout << "Average duration for sort: " << avgDuration1 << " nanoseconds\n";
    std::cout << "\n\n";

    //EMPLACE SORTED
    Internal::sparse_set<int> emplSet{};
    for (int i = 0; i < 10; ++i)
    {
        emplSet.emplace(i, i*10);
    }
    emplSet.sort();

    for (auto it = emplSet.begin(); it != emplSet.end(); ++it)
    {
        std::cout << *it << ", ";
        std::cout << emplSet.sparse_index(it) << "\n";
    }
    std::cout << "\n\n";

    std::less<> compare{};
    emplSet.emplace_sorted(11, compare, 6);

    for (auto it = emplSet.begin(); it != emplSet.end(); ++it)
    {
        std::cout << *it << ", ";
        std::cout << emplSet.sparse_index(it) << "\n";
    }
    std::cout << "\n\n";
}