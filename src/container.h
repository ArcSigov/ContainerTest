/*!
\file container.h
\brief Заголовочный файл с описанием класса и методов класса Container
\authors Аркадий Сигов
*/


#pragma once

#include "value.h"
#include <utility>
#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>

template <typename T>
using element = std::pair<std::string,std::unique_ptr<Value<T>>>;

template <typename T>
using storage = std::vector<element<T>>;

/**
 *  \brief Шаблонный контейнер, устанавливающий значение ключа по определенным правилам формирования ключа
 *  обеспечивает потокобезопасную установку и модификацию переменных, устанавливаемых по значению ключа
*/
template<typename T>
class Container
{
    std::string unused{"DFGJMQV"};  ///! строка символов, которые не должны быть использованы при установке ключа
    storage<T> _s;                  ///! хранилище объектов Value<T>, обеспечивающих работу со значениями
    std::mutex  mute;
public:
    Container() = default;
    ~Container() = default;
    /*!
    \brief Возвращает ссылку на элемент контейнера по ключу, если он соответсвует условиям по формированию ключа
    \param[in] &key ссылка на передаваемый пользователем ключ
    \param[out] &Value ссылка на объект, хранящий ключ
    */
    Value<T>& operator[] (const std::string& key)
    {
        {
            std::lock_guard<std::mutex> lock(mute);
            checkKey(key);
            return findVal(key);
        }
    }
    /*!
    \brief Выполняет инкремент значения ключа, если ключ соответствует условиям по формированию ключа
    \param[in] &key ссылка на передаваемый пользователем ключ
    \param[out] T значение, хранящееся в объекте Value<T>
    */
    T incr(const std::string& key)
    {
        {
            std::lock_guard<std::mutex> lock(mute);
            checkKey(key);
            return findVal(key)++;
        }
    }
    /*!
    \brief Выполняет установку значения ключа, если ключ соответствует условиям по формированию ключа
    \param[in] &key ссылка на передаваемый пользователем ключ
    \param[in] T& значение, устанавливаемое по ключу
    */
    void set(const std::string& key, const T& val)
    {
        {
            std::lock_guard<std::mutex> lock(mute);
            checkKey(key);
            findVal(key) = val;
        }
    }
    /*!
    \brief Выполняет печать контейнера
    \param[in] &out ссылка на поток вывода
    \param[in] Сontainer<T>& контейнер, выводимый на печать
    \param[out] ссылка на поток вывода
    */
    friend std::ostream& operator<<  (std::ostream& out, Container<T>& c)
    {
        {
            std::lock_guard<std::mutex> lock(c.mute);
            for (const auto& it : c._s)
            {
                out << "[" <<it.first << "] = " << it.second->val() << std::endl;
            }
            return out;
        }
    }
private:
    /*!
    \brief Выполняет проверку ключа на правило формирования
    \param[in] key, проверяемый ключ
    */
    void checkKey(std::string key)
    {
        if (key.size() > 30) throw std::length_error("key = " + key + " has different size");
        std::transform(key.begin(),key.end(),key.begin(),[](unsigned char c){return std::toupper(c);});
        std::istringstream s(key);
        for (std::string _s; std::getline(s,_s,'-');)
        {
            if (_s.size() != 2)                                throw std::invalid_argument("key = " + key + " has different size in section");
            else if (!std::isalpha(_s[0]))                     throw std::invalid_argument("key = " + key + " has not alpha in first pos in section");
            else if (unused.find(_s[0]) != std::string::npos)  throw std::invalid_argument("key = " + key + " has a bad symb");
            else if (!std::isdigit(_s[1]))                     throw std::invalid_argument("key = " + key + " has not digit in section");
            else if (_s[1] == '0')                             throw std::invalid_argument("key = " + key + " has digit = 0 in section");
        }
    }
    /*!
    \brief Выполняет поиск значения по установленному ключу
    Если по заданному ключу не будет обнаружено значение, то будет произведена вставка в контейнер нового объекта Value<T>,
    который создаст новый объект, хранящий значение.
    По результатам вставки производится сортировка контейнера по заданным правилам последовательности для устанавливаемых ключей.
    \param[in] &key, ссылка на ключ
    \param[out] &Value<T>, ссылка объект, хранящий и модифицирующий значение
    */
    Value<T>& findVal(const std::string& key)
    {
        auto position = std::find_if(_s.begin(),_s.end(),[&](const element<T>& _pos)
         {
             return _pos.first == key;
         });

        if (position == _s.end())
        {
            _s.emplace_back(key, std::make_unique<Value<T>>());
            std::sort(_s.begin(), _s.end(), [&](const element<T>& e1,const  element<T>& e2)
            {
                auto& str1 = e1.first;
                auto& str2 = e2.first;
                if (str1.size() != str2.size())
                  return str1.size() < str2.size();
                else
                  return str1 < str2;
            });
            position = std::find_if(_s.begin(),_s.end(),[&](const element<T>& _pos) { return _pos.first == key;});
        }
        return *position->second;
    }
};
