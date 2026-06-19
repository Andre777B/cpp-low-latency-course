/*2. Написать counting allocator и показать количество аллокаций при последовательном добавлении элементов в vector*/

#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// Общие счётчики для всех экземпляров аллокатора.
struct AllocationCounter
{
    static inline std::size_t allocations = 0;
    static inline std::size_t deallocations = 0;

    static void reset()
    {
        allocations = 0;
        deallocations = 0;
    }
};

// Простой аллокатор, который считает
// вызовы allocate и deallocate.
template <typename T>
class CountingAllocator
{
public:
    using value_type = T;

    CountingAllocator() = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&)
    {
    }

    T* allocate(std::size_t n)
    {
        ++AllocationCounter::allocations;

        std::wcout << L"Выделение памяти для "
            << n
            << L" элементов\n";

        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* pointer, std::size_t n) noexcept
    {
        ++AllocationCounter::deallocations;

        std::wcout << L"Освобождение памяти для "
            << n
            << L" элементов\n";

        std::allocator<T>{}.deallocate(pointer, n);
    }
};

template <typename T, typename U>
bool operator==(
    const CountingAllocator<T>&,
    const CountingAllocator<U>&
    )
{
    return true;
}

template <typename T, typename U>
bool operator!=(
    const CountingAllocator<T>&,
    const CountingAllocator<U>&
    )
{
    return false;
}

void testWithoutReserve()
{
    AllocationCounter::reset();

    std::wcout << L"\nТест 1. Без reserve\n\n";

    {
        std::vector<int, CountingAllocator<int>> values;

        for (int i = 0; i < 100; ++i)
        {
            values.push_back(i);
        }

        std::wcout << L"\nРазмер вектора: "
            << values.size()
            << L'\n';

        std::wcout << L"Ёмкость вектора: "
            << values.capacity()
            << L'\n';
    }

    std::wcout << L"Количество выделений: "
        << AllocationCounter::allocations
        << L'\n';

    std::wcout << L"Количество освобождений: "
        << AllocationCounter::deallocations
        << L'\n';
}

void testWithReserve()
{
    AllocationCounter::reset();

    std::wcout << L"\nТест 2. С reserve(100)\n\n";

    {
        std::vector<int, CountingAllocator<int>> values;

        // Сразу выделяем память под 100 элементов.
        values.reserve(100);

        for (int i = 0; i < 100; ++i)
        {
            values.push_back(i);
        }

        std::wcout << L"\nРазмер вектора: "
            << values.size()
            << L'\n';

        std::wcout << L"Ёмкость вектора: "
            << values.capacity()
            << L'\n';
    }

    std::wcout << L"Количество выделений: "
        << AllocationCounter::allocations
        << L'\n';

    std::wcout << L"Количество освобождений: "
        << AllocationCounter::deallocations
        << L'\n';
}

int main()
{
    // Настройка русского вывода в консоли Windows.
    _setmode(_fileno(stdout), _O_U16TEXT);

    testWithoutReserve();
    testWithReserve();

    return 0;
}


/*Мы создаём специальный аллокатор, который считает, сколько раз vector просит и освобождает память. 
Затем добавляем 100 элементов сначала без reserve, а потом с reserve(100) и сравниваем результат:
во втором случае память выделяется заранее,
поэтому лишних аллокаций становится меньше.*/
