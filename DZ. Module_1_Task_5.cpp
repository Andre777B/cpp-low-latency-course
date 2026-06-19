/*5. Написать пример, где std::string_view используется безопасно, 
и отдельно пример потенциально опасного использования с пояснением ошибки.*/

#include <iostream>
#include <string>
#include <string_view>

/*
    Безопасный пример.

    Объект std::string существует всё время,
    пока используется string_view.
*/
void safeExample()
{
    std::string text = "Hello, world!";

    /*
        string_view не копирует строку.
        Он только хранит указатель на символы
        и длину исходной строки.
    */
    std::string_view view = text;

    std::cout << "Safe example: "
        << view
        << '\n';

    /*
        Здесь всё правильно:
        объект text ещё существует,
        поэтому view указывает на действительную память.
    */
}

/*
    Опасный пример.

    Функция создаёт локальную строку,
    которая будет уничтожена при выходе из функции.
*/
std::string_view dangerousView()
{
    std::string localText = "Temporary text";

    /*
        view указывает на память строки localText.
    */
    std::string_view view = localText;

    /*
        После return объект localText уничтожается.
        Возвращённый string_view будет указывать
        на память, которой уже нельзя пользоваться.
    */
    return view;
}

int main()
{
    /*
        Запускаем безопасный пример.
    */
    safeExample();

    /*
        Получаем string_view на уже уничтоженную строку.
        Такой объект называется висячим представлением.
    */
    std::string_view badView = dangerousView();

    /*
        Вывод badView является ошибочным использованием.

        Программа может показать правильный текст,
        мусор или вести себя непредсказуемо.
        Поэтому эту строку лучше не выполнять.
    */
    // std::cout << "Dangerous example: " << badView << '\n';

    std::cout
        << "Dangerous example was created, "
        << "but it must not be used.\n";

    return 0;
}

