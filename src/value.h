/*!
\file value.h
\brief Заголовочный файл с описанием класса и методов класса Value
\authors Аркадий Сигов
*/

#pragma once

#include <atomic>
#include <type_traits>
#include <sstream>

template<typename T, class Enable = void>
class Value {};

/**
 *  @brief Шаблонный класс, обеспечивающий работу с целочисленными переменными и с переменной с плавающей точкой
 *  обеспечивает атомарность операций над хранимым значением, если значение переменной модифицируется
  */
template<typename T>
class Value <T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
{
public:
    Value() = default;
    Value(Value& other) = delete;
    Value(Value&&)      = delete;
    Value& operator = (Value& other) = delete;
    Value& operator = (Value&&) = delete;
    ~Value() = default;
    /*!
    \brief Возвращает значение элемента после его инкрементирования
    \param[out] T значение типа
    */
    T operator++ (T)
    {
        return ++value;
    }
    /*!
    \brief Перегрузка оператора =, возвращающее значение переменной, после установки в переменную значения от other
    \param[in] &other ссылка на значение, которое запишется в value
    \param[out] T значение типа
    */
    T operator=(const T& other)
    {
        value.store(other);
        return value;
    }
    /*!
    \brief Возвращает хранимое значение объекта
    \param[out] T значение типа
    */
    T val()
    {
        return value;
    }
    /*!
    \brief Выполняет печать значения
    \param[in] &out ссылка на поток вывода
    \param[in] Value<T>& объект, хранящий значение
    \param[out] ссылка на поток вывода
    */
    friend std::ostream& operator << (std::ostream& out, const Value<T>& t)
    {
        return out << t.value;
    }
private:
    std::atomic<T> value;   ///!< атомарная переменная, обеспечивающая работу с переменной при использовании в различных потоках
};


template<typename T>
class Value <T, std::enable_if_t<std::is_class_v<T>>> {
};
