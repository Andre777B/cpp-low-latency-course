/*3. Сделать небольшой benchmark: сравнить обычное накопление строк и накопление через заранее подготовленный буфер.*/


#include <chrono>
#include <iostream>
#include <string>

/*
    Количество символов, которые будем добавлять в строку.
    Чем больше значение, тем заметнее может быть разница.
*/
const int SYMBOL_COUNT = 1'000'000;

/*
    Первый тест.

    Строка создаётся пустой, и память заранее не резервируется.
    При увеличении строки std::string может несколько раз
    выделять новый буфер и переносить в него старые символы.
*/
long long testWithoutReserve()
{
    std::string text;

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < SYMBOL_COUNT; ++i)
    {
        text += 'A';
    }

    auto finish = std::chrono::steady_clock::now();

    /*
        Используем размер строки, чтобы компилятор
        не удалил всю работу как ненужную.
    */
    std::cout << "String size without reserve: "
        << text.size()
        << '\n';

    return std::chrono::duration_cast<std::chrono::microseconds>(
        finish - start
    ).count();
}

/*
    Второй тест.

    Перед заполнением строки вызывается reserve().
    Метод заранее выделяет память под нужное количество символов.
    Благодаря этому строке не нужно постоянно увеличивать буфер.
*/
long long testWithReserve()
{
    std::string text;

    /*
        Заранее резервируем память под миллион символов.
        Размер строки после reserve всё ещё равен нулю.
    */
    text.reserve(SYMBOL_COUNT);

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < SYMBOL_COUNT; ++i)
    {
        text += 'A';
    }

    auto finish = std::chrono::steady_clock::now();

    std::cout << "String size with reserve: "
        << text.size()
        << '\n';

    return std::chrono::duration_cast<std::chrono::microseconds>(
        finish - start
    ).count();
}

int main()
{
    /*
        Запускаем оба варианта и сохраняем время работы.
        Время измеряется в микросекундах.
    */
    long long timeWithoutReserve = testWithoutReserve();
    long long timeWithReserve = testWithReserve();

    std::cout << "\nTime without reserve: "
        << timeWithoutReserve
        << " microseconds\n";

    std::cout << "Time with reserve: "
        << timeWithReserve
        << " microseconds\n";

    /*
        Меньшее время означает более быстрое выполнение.
        Обычно вариант с reserve работает стабильнее,
        потому что память выделяется заранее.
    */
    if (timeWithReserve < timeWithoutReserve)
    {
        std::cout << "\nThe version with reserve was faster.\n";
    }
    else
    {
        std::cout << "\nThe result may vary. Run the program several times.\n";
    }

    return 0;
}

/*в первом тесте строка увеличивается без заранее выделенной памяти, поэтому её буфер может несколько 
раз перераспределяться. Во втором тесте reserve() заранее подготавливает память,
после чего сравнивается время выполнения двух вариантов*/