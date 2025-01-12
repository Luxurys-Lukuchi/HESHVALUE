#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <utility>
#include <string>
#include <cassert>
// avtor Luxurys_Lukuchi
// Функция для вычисления хеша строки с использованием алгоритма DJB2
unsigned long hash_djb2(const std::string& str) {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Шаблонный класс HashTable, который принимает тип значения и хеш-функции
template <typename Value, typename HashFunc = std::hash<Value>>
class HashTable {
public:
    // Конструктор с возможностью задания начального размера таблицы и хеш-функции
    HashTable(size_t size = 100, HashFunc hash_func = HashFunc())
        : table(size), hash_function(hash_func), current_size(0), max_load_factor(0.7), min_load_factor(0.2) {}

    // Метод insert вставляет значение в таблицу.
    // Если текущий размер таблицы превышает максимальный коэффициент заполнения,
    // таблица рехешируется для увеличения размера.
    void insert(const Value& value) {
        if (current_size >= table.size() * max_load_factor) {
            rehash(); // Если текущий размер превышает максимальный коэффициент заполнения, рехешируем таблицу
        }
        size_t index = hash_function(value) % table.size(); // Вычисляем индекс для вставки
        table[index].emplace_back(value); // Вставляем значение в соответствующий список
        current_size++; // Увеличиваем текущий размер
    }

    // Метод find ищет элемент в таблице по значению.
    // Если элемент найден, метод возвращает true.
    // Если элемент не найден, метод возвращает false.
    bool find(const Value& value) const {
        size_t index = hash_function(value) % table.size(); // Вычисляем индекс для поиска
        for (const auto& elem : table[index]) { // Проходим по списку в ячейке
            if (elem == value) { // Если значение совпадает
                return true; // Возвращаем true
            }
        }
        return false; // Если значение не найдено, возвращаем false
    }

    // Метод remove удаляет элемент из таблицы по значению.
    // Если текущий размер таблицы меньше минимального коэффициента заполнения,
    // таблица рехешируется для уменьшения размера.
    void remove(const Value& value) {
        size_t index = hash_function(value) % table.size(); // Вычисляем индекс для удаления
        auto& cell = table[index]; // Получаем ссылку на список в ячейке
        cell.remove_if([&](const Value& elem) { // Удаляем элемент, если значение совпадает
            return elem == value;
        });
        current_size--; // Уменьшаем текущий размер
        if (current_size < table.size() * min_load_factor) {
            rehash_down(); // Если текущий размер меньше минимального коэффициента заполнения, рехешируем таблицу вниз
        }
    }

    // Метод size возвращает текущее количество элементов в таблице.
    size_t size() const {
        return current_size; // Возвращаем текущий размер
    }

    // Метод load_factor возвращает текущий коэффициент заполнения таблицы.
    double load_factor() const {
        return static_cast<double>(current_size) / table.size(); // Возвращаем коэффициент заполнения
    }

    // Метод getBucketSize возвращает размер списка в ячейке таблицы по значению.
    size_t getBucketSize(const Value& value) const {
        size_t index = hash_function(value) % table.size();
        return table[index].size();
    }

    // Метод values возвращает вектор всех значений в таблице.
    std::vector<Value> values() const {
        std::vector<Value> all_values;
        for (const auto& cell : table) { // Проходим по всем ячейкам таблицы
            for (const auto& elem : cell) { // Проходим по всем элементам в ячейке
                all_values.push_back(elem); // Добавляем значение в вектор
            }
        }
        return all_values;
    }

private:
    std::vector<std::list<Value>> table; // Вектор списков для хранения элементов
    HashFunc hash_function; // Хеш-функция
    size_t current_size; // Текущее количество элементов в таблице
    double max_load_factor; // Максимальный коэффициент заполнения
    double min_load_factor; // Минимальный коэффициент заполнения

    // Метод rehash увеличивает размер таблицы и перераспределяет элементы.
    void rehash() {
        size_t new_size = table.size() * 2; // Удваиваем размер таблицы
        std::vector<std::list<Value>> new_table(new_size); // Создаем новую таблицу
        for (const auto& cell : table) { // Проходим по всем ячейкам старой таблицы
            for (const auto& elem : cell) { // Проходим по всем элементам в ячейке
                size_t index = hash_function(elem) % new_size; // Вычисляем новый индекс
                new_table[index].emplace_back(elem); // Вставляем элемент в новую таблицу
            }
        }
        table = std::move(new_table); // Перемещаем новую таблицу в старую
    }

    // Метод rehash_down уменьшает размер таблицы и перераспределяет элементы.
    void rehash_down() {
        size_t new_size = table.size() / 2; // Уменьшаем размер таблицы вдвое
        std::vector<std::list<Value>> new_table(new_size); // Создаем новую таблицу
        for (const auto& cell : table) { // Проходим по всем ячейкам старой таблицы
            for (const auto& elem : cell) { // Проходим по всем элементам в ячейке
                size_t index = hash_function(elem) % new_size; // Вычисляем новый индекс
                new_table[index].emplace_back(elem); // Вставляем элемент в новую таблицу
            }
        }
        table = std::move(new_table); // Перемещаем новую таблицу в старую
    }
};

void test() {
    // Тесты для строковых значений
    HashTable<std::string, std::function<unsigned long(const std::string&)>> hashTable(100, [](const std::string& key) {
        return 42; // Все значения отображаются на один и тот же индекс
    });

    // Вставка элементов
    hashTable.insert("value1");
    hashTable.insert("value2");

    // Проверка вставки
    assert(hashTable.size() == 2);

    // Поиск элемента
    assert(hashTable.find("value1"));
    assert(hashTable.find("value2"));

    // Удаление элемента
    hashTable.remove("value1");
    assert(hashTable.size() == 1);
    assert(!hashTable.find("value1"));

    // Повторная вставка и проверка
    hashTable.insert("value1");
    assert(hashTable.size() == 2);
    assert(hashTable.find("value1"));

    // Проверка рехеширования
    for (long long i = 3; i <= 150; ++i) {
        hashTable.insert("value" + std::to_string(i));
    }
    assert(hashTable.size() == 150);
    assert(hashTable.load_factor() <= 0.7);

    // Проверка рехеширования вниз
    for (long long i = 3; i <= 150; ++i) {
        hashTable.remove("value" + std::to_string(i));
    }
    assert(hashTable.size() == 2);
    assert(hashTable.load_factor() >= 0.2);

    // Проверка хеш-функции
    assert(hash_djb2("value1") == 574111667);
    assert(hash_djb2("") == 5381);
    assert(hash_djb2("test_string") == 4175666075);

    // Тесты для числовых значений
    HashTable<long long> intHashTable;

    // Вставка элементов
    intHashTable.insert(1LL);
    intHashTable.insert(2LL);

    // Проверка вставки
    assert(intHashTable.size() == 2);

    // Поиск элемента
    assert(intHashTable.find(1LL));
    assert(intHashTable.find(2LL));

    // Удаление элемента
    intHashTable.remove(1LL);
    assert(intHashTable.size() == 1);
    assert(!intHashTable.find(1LL));

    // Повторная вставка и проверка
    intHashTable.insert(1LL);
    assert(intHashTable.size() == 2);
    assert(intHashTable.find(1LL));

    // Проверка наличия коллизий для строковых значений
    hashTable.insert("collision1");
    hashTable.insert("collision2");
    assert(hashTable.getBucketSize("collision1") > 1); // Проверка, что в ячейке более одного элемента

    // Проверка, что оба элемента можно найти
    assert(hashTable.find("collision1"));
    assert(hashTable.find("collision2"));

    hashTable.insert("collision3");
    hashTable.insert("collision4");
    assert(hashTable.getBucketSize("collision1") > 3); // Проверка, что в ячейке более трех элементов

    // Проверка, что все элементы можно найти
    assert(hashTable.find("collision3"));
    assert(hashTable.find("collision4"));

    // Удаление элементов и проверка
    hashTable.remove("collision1");
    hashTable.remove("collision2");
    hashTable.remove("collision3");
    hashTable.remove("collision4");
    assert(hashTable.size() == 2);
    assert(!hashTable.find("collision1"));
    assert(!hashTable.find("collision2"));
    assert(!hashTable.find("collision3"));
    assert(!hashTable.find("collision4"));

    // Проверка наличия коллизий для числовых значений
    HashTable<long long, std::function<size_t(const long long&)>> intHashTableWithCollisions(100, [](const long long& key) {
        return 42; // Все значения отображаются на один и тот же индекс
    });

    intHashTableWithCollisions.insert(100LL);
    intHashTableWithCollisions.insert(200LL);
    assert(intHashTableWithCollisions.getBucketSize(100LL) > 1); // Проверка, что в ячейке более одного элемента

    // Проверка, что оба элемента можно найти
    assert(intHashTableWithCollisions.find(100LL));
    assert(intHashTableWithCollisions.find(200LL));

    intHashTableWithCollisions.insert(300LL);
    intHashTableWithCollisions.insert(400LL);
    assert(intHashTableWithCollisions.getBucketSize(100LL) > 3); // Проверка, что в ячейке более трех элементов

    // Проверка, что все элементы можно найти
    assert(intHashTableWithCollisions.find(300LL));
    assert(intHashTableWithCollisions.find(400LL));

    // Удаление элементов и проверка
    intHashTableWithCollisions.remove(100LL);
    intHashTableWithCollisions.remove(200LL);
    intHashTableWithCollisions.remove(300LL);
    intHashTableWithCollisions.remove(400LL);
    assert(intHashTableWithCollisions.size() == 0);
    assert(!intHashTableWithCollisions.find(100LL));
    assert(!intHashTableWithCollisions.find(200LL));
    assert(!intHashTableWithCollisions.find(300LL));
    assert(!intHashTableWithCollisions.find(400LL));

    std::cout << "All tests complete" << std::endl;
}
