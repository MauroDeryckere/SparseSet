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
    Internal::SparseSet<int> set{ };
}

void TestSparseSetEmplace()
{
    Internal::SparseSet<int> set{ };

}
