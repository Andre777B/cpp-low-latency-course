/* Домашнее задание. Модуль 1. Задание 1
* 

1. Реализовать простой объектный пул для структуры Order или Trade с использованием placement new и ручного уничтожения объектов.
*/

#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <new>
#include <utility>

// Заявка, объекты которой будут храниться в пуле.
struct Order
{
    int id;
    double price;

    Order(int orderId, double orderPrice)
        : id(orderId),
        price(orderPrice)
    {
        std::wcout << L"Создана заявка: id = "
            << id
            << L", цена = "
            << price
            << L'\n';
    }

    ~Order()
    {
        std::wcout << L"Удалена заявка: id = "
            << id
            << L'\n';
    }
};

// Одна ячейка сырой памяти.
// Памяти достаточно для одного объекта типа T,
// но сам объект здесь пока не создан.
template <typename T>
struct PoolSlot
{
    alignas(T) std::byte memory[sizeof(T)];
};

// Объектный пул фиксированной ёмкости.

// T - тип объектов.
// Capacity - максимальное количество объектов.
template <typename T, std::size_t Capacity>
class ObjectPool
{
public:
    ObjectPool() = default;

    // Копирование пула запрещаем.
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    // Перемещение пула также запрещаем,
    // потому что внутри находятся живые объекты.
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    // Создание объекта в первой свободной ячейке.
    template <typename... Args>
    T* create(Args&&... args)
    {
        for (std::size_t i = 0; i < Capacity; ++i)
        {
            if (!used_[i])
            {
                void* address =
                    static_cast<void*>(slots_[i].memory);

                // Placement new создаёт объект
                // в уже выделенной памяти.
                T* object = ::new (address)
                    T(std::forward<Args>(args)...);

                used_[i] = true;

                return object;
            }
        }

        // Свободных ячеек нет.
        return nullptr;
    }

    // Уничтожение объекта.
    bool destroy(T* object) noexcept
    {
        if (object == nullptr)
        {
            return false;
        }

        for (std::size_t i = 0; i < Capacity; ++i)
        {
            if (used_[i] && getPointer(i) == object)
            {
                // Вызываем деструктор вручную.
                std::destroy_at(object);

                // Память остаётся внутри пула,
                // а ячейка становится свободной.
                used_[i] = false;

                return true;
            }
        }

        // Объект не найден или уже был уничтожен.
        return false;
    }

    // Количество живых объектов.
    std::size_t size() const noexcept
    {
        std::size_t count = 0;

        for (bool isUsed : used_)
        {
            if (isUsed)
            {
                ++count;
            }
        }

        return count;
    }

    // Максимальная ёмкость пула.
    constexpr std::size_t capacity() const noexcept
    {
        return Capacity;
    }

    // При уничтожении пула уничтожаем
    // все оставшиеся живые объекты.
    ~ObjectPool()
    {
        for (std::size_t i = 0; i < Capacity; ++i)
        {
            if (used_[i])
            {
                std::destroy_at(getPointer(i));
                used_[i] = false;
            }
        }
    }

private:
    // Получение указателя на объект в ячейке.
    //
    // Метод используется только тогда,
    // когда в ячейке уже создан живой объект.
    T* getPointer(std::size_t index) noexcept
    {
        T* pointer =
            reinterpret_cast<T*>(slots_[index].memory);

        return std::launder(pointer);
    }

    // Сырая память под объекты.
    std::array<PoolSlot<T>, Capacity> slots_{};

    // Состояние каждой ячейки:
    // false - свободна;
    // true  - занята.
    std::array<bool, Capacity> used_{};
};

int main()
{
    // Переводим стандартный вывод Windows
    // в режим Unicode UTF-16.
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Создаём пул максимум для трёх заявок.
    ObjectPool<Order, 3> pool;

    std::wcout << L"Ёмкость пула: "
        << pool.capacity()
        << L"\n\n";

    // Создаём три объекта в памяти пула.
    Order* firstOrder =
        pool.create(1, 101.25);

    Order* secondOrder =
        pool.create(2, 102.50);

    Order* thirdOrder =
        pool.create(3, 103.75);

    std::wcout << L"\nОбъектов в пуле: "
        << pool.size()
        << L'\n';

    // Выводим данные первой заявки.
    if (firstOrder != nullptr)
    {
        std::wcout << L"Первая заявка: id = "
            << firstOrder->id
            << L", цена = "
            << firstOrder->price
            << L'\n';
    }

    // Пытаемся создать четвёртый объект.
    // Пул уже заполнен.
    Order* extraOrder =
        pool.create(4, 104.00);

    if (extraOrder == nullptr)
    {
        std::wcout
            << L"\nСвободных мест в пуле нет\n";
    }

    // Уничтожаем вторую заявку.
    std::wcout
        << L"\nОсвобождаем вторую заявку\n";

    if (pool.destroy(secondOrder))
    {
        secondOrder = nullptr;

        std::wcout
            << L"Вторая заявка успешно удалена\n";
    }
    else
    {
        std::wcout
            << L"Не удалось удалить вторую заявку\n";
    }

    std::wcout << L"Объектов после удаления: "
        << pool.size()
        << L'\n';

    // Создаём новый объект.
    // Он займёт освободившуюся ячейку.
    std::wcout
        << L"\nПовторно используем свободную ячейку\n";

    Order* fourthOrder =
        pool.create(4, 104.00);

    if (fourthOrder != nullptr)
    {
        std::wcout << L"Новая заявка: id = "
            << fourthOrder->id
            << L", цена = "
            << fourthOrder->price
            << L'\n';
    }

    std::wcout
        << L"\nИтоговое количество объектов: "
        << pool.size()
        << L"\n\n";

    // firstOrder, thirdOrder и fourthOrder
    // вручную не уничтожаем.
    
    // При завершении main деструктор pool
    // автоматически уничтожит оставшиеся объекты.

    return 0;
}


/*реализован объектный пул фиксированной ёмкости, который создаёт объекты Order в заранее выделенной памяти с помощью placement new 
и уничтожает их через std::destroy_at. 
Освобождённые ячейки повторно используются без обычных вызовов new и delete.*/
