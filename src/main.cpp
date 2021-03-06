#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <iostream>
#include <chrono>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{

using string = std::string;
using HashMap = aisdi::HashMap<int, string>;
using TreeMap = aisdi::TreeMap<int, string>;
const string testString = "dummy value";

void performTreeMapInsertingTest(size_t howManyInserts)
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,50000);

    TreeMap map;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout << "HashMap\t\tInsert\t\t" << howManyInserts << "\t\t";

    start = std::chrono::system_clock::now();
    for (size_t i = 0; i< howManyInserts; ++i)
    {
        int number = distribution(generator);
        map[number] = testString;
    }
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> timeTaken = end-start;
    std::cout << timeTaken.count() << "s\n";
}

void performHashMapInsertingTest(size_t howManyInserts)
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,50000);

    HashMap map;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout << "TreeMap\t\tInsert\t\t" << howManyInserts << "\t\t";

    start = std::chrono::system_clock::now();
    for (size_t i = 0; i< howManyInserts; ++i)
    {
        int number = distribution(generator);
        map[number] = testString;
    }
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> timeTaken = end-start;
    std::cout << timeTaken.count() << "s\n";
}

void performTreeMapIterationTest(size_t howManyElements)
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,50000);

    TreeMap map;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout << "HashMap\t\tIterate\t\t" << howManyElements << "\t\t";

	for (size_t i = 0; i< howManyElements; ++i)
    {
        int number = distribution(generator);
        map[number] = testString;
    }
    start = std::chrono::system_clock::now();
    for(auto elem: map)
    {

	}
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> timeTaken = end-start;
    std::cout << timeTaken.count() << "s\n";
}

void performHashMapIterationTest(size_t howManyElements)
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,50000);

    HashMap map;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout << "TreeMap\t\tIterate\t\t" << howManyElements << "\t\t";

	for (size_t i = 0; i< howManyElements; ++i)
    {
        int number = distribution(generator);
        map[number] = testString;
    }
    start = std::chrono::system_clock::now();
    for(auto elem: map)
    {

	}
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> timeTaken = end-start;
    std::cout << timeTaken.count() << "s\n";
}

void line(size_t width = 64)
{
    for(size_t i = 0; i < width; i++)
        std::cout << '=';
    std::cout << '\n';
}

} // namespace

int main()
{
    line();
    std::cout << "\tTime tests for TreeMap and HashMap\n";
    std::cout << "\tNormal distribution is used to generate keys\n";
    line();
    std::cout << "Variant:\tOperation:\tCount:\t\tTime taken:\n";
    line(); // names were swapped accidentally
    performTreeMapInsertingTest(100);
    performHashMapInsertingTest(100);
    line();
    performTreeMapInsertingTest(1000);
    performHashMapInsertingTest(1000);
    line();
    performTreeMapInsertingTest(10000);
    performHashMapInsertingTest(10000);
    line();
    performTreeMapIterationTest(100);
    performHashMapIterationTest(100);
    line();
    performTreeMapIterationTest(1000);
    performHashMapIterationTest(1000);
    line();
    performTreeMapIterationTest(10000);
    performHashMapIterationTest(10000);
    line();
    return 0;
}
