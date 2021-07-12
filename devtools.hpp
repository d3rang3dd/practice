#ifndef DEVTOOLS_HPP
#define DEVTOOLS_HPP

#include <iostream>
#include <typeinfo>
#include <utility>
#include <iterator>
#include <cxxabi.h>

namespace DEVTOOLS
{
    // обертка вокруг std::cout с возможностью гасить отладочную информацию
    class DebugCout : private std::ostream
    {
        private:
            bool suppressed = false;

        public:
            template <typename T>
            std::ostream& operator<< (const T& to_cout)
            {
                if (! suppressed)
                    return std::cout << to_cout;
                return *this;
            }

            template <typename T>
            std::ostream& operator<< (T&& to_cout)
            {
                if (! suppressed)
                    return std::cout << to_cout;
                return *this;
            }

            void suppress(bool sup = true)
            {
                suppressed = sup;
            }

    } debug_cout;

    // пара, в которой пор€док элементов не важен
    template <typename T>
    struct symmetric_pair
    {
        T first, second;

        symmetric_pair () = delete;

        symmetric_pair (const T& x, const T& y)
          : first {x},
            second {y} {};

        symmetric_pair (const std::pair<T, T>& p)
          : first {p.first},
            second {p.second} {};

        symmetric_pair (std::pair<T, T>&& p)
          : first {p.first},
            second {p.second} {};

        bool contains(T x, T y)
        {
            return (first == x && second == y) || (first == y && second == x);
        }

        bool contains(const std::pair<T, T>& p)
        {
            return contains(p.first, p.second);
        }

        bool operator== (const symmetric_pair<T>& sp)
        {
            return contains(sp.first, sp.second);
        }

        bool operator!= (const symmetric_pair<T>& sp)
        {
            return !(*this == sp);
        }

        bool contains(const symmetric_pair<T>& sp)
        {
            return *this == sp;
        }

        symmetric_pair& reverse()
        {
            std::swap(first, second);
            return *this;
        }
    };

    // печать контейнера

    // есть ли в контейнере
    // добавить const_reference
    template <typename Container, typename T>
    bool contains(Container& cont, T& x)
    {
        auto res = std::find(cont.begin(), cont.end(), x);
        return res != cont.end();
    }

    #if __GNUG__  // если используетс€ GCC

    // функци€, позвол€юща€ получить строку с типом переменной,
    // а также расшифровать псевдоним (если он поддерживает default-init)
    // ! добавить поддержку концептов
    template <typename T>
    std::string&& getTypeName(const T& x)
    {
        int status;
        std::string type;
        try
            { type = typeid(x).name(); }
        catch (std::bad_typeid& exc)
            { return exc.what(); }

        __cxxabiv1::__cxa_demangle(type.c_str(), 0, 0, &status);

        switch (status)
        {
            case 0 : return std::move(type);
            case -1 :
                return "__cxa_demangle: A memory allocation failure occured";
            case -2 :
                return "__cxa_demangle: Mangled_name is not a valid "
                    "name under the C++ ABI mangling rules";
            case -3 :
                return "__cxa_demangle: One of the arguments is invalid";
        }
    }

    #endif  // если используетс€ GCC

} // namespace DEVTOOLS

#endif // DEVTOOLS_HPP