#include <iostream>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <memory>
#include <vector>

using namespace std;


template<typename T, class Enable = void>
class Value {};

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
    T operator++ (T)
    {
       return ++value;
    }
    T operator=(const T& other)
    {
        value.store(other);
        return value;
    }
    T val()
    {
        return value;
    }
    friend std::ostream& operator << (std::ostream& out, const Value<T>& t)
    {
        return out << t.value;
    }
private:
    std::atomic<T> value;
};

template<typename T>
class Value <T, std::enable_if_t<std::is_class_v<T>>> {};

template <typename T>
using element = std::pair<std::string,std::unique_ptr<Value<T>>>;

template <typename T>
using storage = std::vector<element<T>>;

template <typename T>
using pos = typename storage<T>::iterator;

template<typename T>
class Container : Value<T>
{
    std::string unused{"DFGJMQV"};
    storage<T> _s;
    std::mutex  mute;
public:
    Container() = default;
    ~Container() = default;

    Value<T>& operator[] (const std::string& key)
    {
        std::lock_guard<std::mutex> lock(mute);
        checkKey(key);
        return findVal(key);
    }

    T incr(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(mute);
        checkKey(key);
        return findVal(key)++;
    }

    void set(const std::string& key, const T& val)
    {
        std::lock_guard<std::mutex> lock(mute);
        checkKey(key);
        findVal(key) = val;
    }

    friend std::ostream& operator<<  (std::ostream& out, Container<T>& c)
    {
        std::lock_guard<std::mutex> lock(c.mute);
        for (const auto& it : c._s)
        {
            out << it.first << " " << it.second->val() << std::endl;
        }
        return out;
    }
private:
    void checkKey(std::string key)
    {
        std::transform(key.begin(),key.end(),key.begin(),[](unsigned char c){return std::toupper(c);});
        std::istringstream s(key);
        for (std::string _s; std::getline(s,_s,'-');)
        {
            if (_s.size() != 2 ||
                !std::isalpha(_s[0]) || unused.find(_s[0]) != std::string::npos ||
                !std::isdigit(_s[1]) || _s[1] == '0')
            {
                throw std::invalid_argument("key = " + key + " has invalid character");
            }
        }
    }
    Value<T>& findVal(const std::string& key)
    {
        auto position = std::find_if(_s.begin(),_s.end(),[&](element<T>& _pos)
        {
            return _pos.first == key;
        });

        if (position == _s.end())
        {
            _s.emplace_back(key, std::make_unique<Value<T>>());
            std::sort(_s.begin(), _s.end(), [&](element<T>& e1, element<T>& e2)
            {
                bool flag;
                auto k1 = e1.first;
                auto k2 = e2.first;
                if (k1.size() == k2.size())
                {
                    return k1 < k2;
                }
                else
                {
                    return
                }




                //return size;
            });
        }

        position = std::find_if(_s.begin(),_s.end(),[&](element<T>& _pos)
        {
            return _pos.first == key;
        });

        return *position->second;
    }
};


int main()
{
    std::vector<int> m;

    std::vector<int>::iterator t = m.begin();

    try {
        Container<int> c;

        c["A1"]=120;
        c["A1"]++;
        c.set("A2-A2",320);
        c.set("A2",320);
        c.set("A3",320);
        c.set("A1-A2",320);
        c.incr("A1-A3");
        c.incr("A2-A4");

//        std::cout << "A1=" << c["A1"] << std::endl;
        std::cout << c << std::endl;

    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    cout << "Hello World!" << endl;
    return 0;
}
