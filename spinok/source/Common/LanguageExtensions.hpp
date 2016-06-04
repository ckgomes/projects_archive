#ifndef LANGUAGE_EXTENSIONS_HPP
#define LANGUAGE_EXTENSIONS_HPP

#include <tuple>

// Note that you can still assign different types to eachother
// If their underlying type is the same
#define STRONG_TYPEDEF(type, underlying)                 \
class type                                               \
{                                                        \
    underlying value;                                    \
public:                                                  \
    template <typename... Args>                          \
    type(Args... args) : value {args...} {}              \
    operator underlying&() { return value; }             \
    operator const underlying&() const { return value; } \
};

template <typename L>
struct ScopeGuard
{
    const L fn;
    ScopeGuard(L fn) : fn {fn}
    {}
    ~ScopeGuard()
    {
        fn();
    }
};

#define EXPAND_PP(line, task) auto _lambda_ ## line = [&]() { task; }; ScopeGuard<decltype(_lambda_ ## line)> _lambda_obj_ ## line(_lambda_ ## line)
#define EXPAND_AR(line, task) EXPAND_PP(line, task)
#define defer(task) EXPAND_AR(__LINE__, task)

namespace ext
{

template<std::size_t I = 0, typename FuncT, typename... Tp>
constexpr inline typename std::enable_if<I == sizeof...(Tp), void>::type
for_each(std::tuple<Tp...>&, FuncT)
{}
template<std::size_t I = 0, typename FuncT, typename... Tp>
constexpr inline typename std::enable_if<I<sizeof...(Tp), void>::type
    for_each(std::tuple<Tp...>& t, FuncT f)
{
    f(std::get<I>(t));
    ext::for_each<I + 1, FuncT, Tp...>(t, f);
}
template <class T, class Tuple>
struct index_of;
template <class T, class... Types>
struct index_of<T, std::tuple<T, Types...>>
{
    static const std::size_t value = 0;
};
template <class T, class U, class... Types>
struct index_of<T, std::tuple<U, Types...>>
{
    static const std::size_t value = 1 + index_of<T, std::tuple<Types...>>::value;
};

template <typename T, typename Tuple>
struct has_type;
template <typename T>
struct has_type<T, std::tuple<>> : std::false_type
{};
template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>>
{};
template <typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type
{};

template <typename T, typename Tuple>
using tuple_contains_type = typename has_type<T, Tuple>::type;

}

#endif // LANGUAGE_EXTENSIONS_HPP

