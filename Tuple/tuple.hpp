#pragma once


template<typename... T>
class Tuple;

template<typename HeadT, typename... TailT>
class Tuple<HeadT, TailT...> : public Tuple<TailT...> {

public:

    using Head = HeadT;
    using Tail = Tuple<TailT...>;

    Tuple() = default;

    Tuple(HeadT head, TailT... tail) : Tuple<TailT...>(tail...), value(head) {}

    void swap(Tuple& other) noexcept {
        std::swap(*this, other);
    }

    template <size_t n, typename ret_type>
    constexpr ret_type& get_val() {
        if constexpr (n == 0) {
            return value;
        }
        else {
            return Tail::template get_val<n - 1, ret_type>();
        }
    }

    template <size_t n, typename ret_type>
    constexpr const ret_type& get_val() const {
        if constexpr (n == 0) {
            return value;
        } 
        else {
            return Tail::template get_val<n - 1, ret_type>();
        }
    }

private:
    HeadT value;

};


template<>
class Tuple<> {};


template <typename... Args>
constexpr auto make_tuple(Args&&... args) {
    return Tuple<Args...>(std::forward<Args>(args)...);
}


template <typename... Args>
struct std::tuple_size<Tuple<Args...>> : std::integral_constant<size_t, sizeof...(Args)> {};

template <typename... Args>
struct std::tuple_size<const Tuple<Args...>> : std::integral_constant<size_t, sizeof...(Args)> {};

template <typename... Args>
struct std::tuple_size<Tuple<Args...>&> : std::integral_constant<size_t, sizeof...(Args)> {};

template <typename... Args>
struct std::tuple_size<const Tuple<Args...>&> : std::integral_constant<size_t, sizeof...(Args)> {};

template<typename T>
constexpr size_t tuple_size_v = std::tuple_size<T>::value;


template <size_t n, typename TUPLE>
struct get_type {
    using type = typename get_type<n - 1, typename TUPLE::Tail>::type;
};

template <typename TUPLE>
struct get_type<0, TUPLE> {
    using type = typename TUPLE::Head;
};


template <size_t n, typename... T>
struct std::tuple_element<n, Tuple<T...>> : get_type<n, Tuple<T...>> {};

template <size_t n, typename... T>
struct std::tuple_element<n, const Tuple<T...>> : get_type<n, Tuple<T...>> {};

template <size_t n, typename... T>
struct std::tuple_element<n, Tuple<T...>&> : get_type<n, Tuple<T...>> {};

template <size_t n, typename... T>
struct std::tuple_element<n, const Tuple<T...>&> : get_type<n, Tuple<T...>> {};

template <size_t n, typename... T>
using tuple_element_t = typename std::tuple_element<n, T...>::type;


template <size_t n>
constexpr decltype(auto) get_elem(auto&& tuple) {
    using type = std::remove_reference_t<tuple_element_t<n, decltype(tuple)>>;
    return tuple.template get_val<n, type>();
}

template <typename T, size_t n = 0>
constexpr decltype(auto) get_elem(auto&& tuple) {
    using type = std::remove_reference_t<tuple_element_t<n, decltype(tuple)>>;
    if constexpr (std::is_same_v<type, std::remove_reference_t<T>>) {
        return tuple.template get_val<n, type>();
    } 
    else {
        return get_elem<T, n + 1>(tuple);
    }
}

template<size_t T, typename... Types >
constexpr auto& get(Tuple<Types...>& t) noexcept {
    return get_elem<T>(t);
}

template<size_t T, typename... Types >
constexpr auto&& get(Tuple<Types...>&& t) noexcept {
    return get_elem<T>(t);
}

template<size_t T, typename... Types >
constexpr const auto& get(const Tuple<Types...>& t) noexcept {
    return get_elem<T>(t);
}

template<size_t T, typename... Types >
constexpr const auto&& get(const Tuple<Types...>&& t) noexcept {
    return get_elem<T>(t);
}

template<typename T, typename... Types >
constexpr auto& get(Tuple<Types...>& t) noexcept {
    return get_elem<T>(t);
}

template<typename T, typename... Types >
constexpr auto&& get(Tuple<Types...>&& t) noexcept {
    return get_elem<T>(t);
}

template<typename T, typename... Types >
constexpr const auto& get(const Tuple<Types...>& t) noexcept {
    return get_elem<T>(t);
}

template<typename T, typename... Types >
constexpr const auto&& get(const Tuple<Types...>&& t) noexcept {
    return get_elem<T>(t);
}


template <size_t n = 0, typename... T1, typename... T2>
std::strong_ordering operator<=>(const Tuple<T1...>& tuple1, const Tuple<T2...>& tuple2) {
    if constexpr (n >= std::min(std::tuple_size<decltype(tuple1)>::value, std::tuple_size<decltype(tuple2)>::value)) {
        return std::tuple_size<decltype(tuple1)>::value <=> std::tuple_size<decltype(tuple2)>::value;
    } 
    else {
        if (get<n>(tuple1) != get<n>(tuple2)) {
            return get<n>(tuple1) <=> get<n>(tuple2);
        } 
        else {
            return operator<=><n + 1>(tuple1, tuple2);
        }
    }
}

template <typename... T1, typename... T2>
auto operator==(const Tuple<T1...>& tuple1, const Tuple<T2...>& tuple2) {
  return (tuple1 <=> tuple2) == 0;
}


template <size_t n, typename... T>
auto add_elem(Tuple<T...>) {
    return Tuple<T..., std::integral_constant<size_t, n>>();
}

template <size_t n>
struct get_n_elem {
    using type = decltype(add_elem<n - 1>(typename get_n_elem<n - 1>::type()));
};

template <>
struct get_n_elem<0> {
    using type = Tuple<>;
};

template <typename... T1, typename... T2>
constexpr auto operator+(const Tuple<T1...>& tuple1, const Tuple<T2...>& tuple2) {
    return cat_two(tuple1, tuple2, typename get_n_elem<sizeof...(T1)>::type()
                                    ,typename get_n_elem<sizeof...(T2)>::type());
}

template <typename... T1, typename... T2, typename... T3, typename... T4>
constexpr auto cat_two(const Tuple<T1...>& tuple1, const Tuple<T2...>& tuple2, Tuple<T3...>, Tuple<T4...>) {
    return Tuple<T1..., T2...>(get<T3::value>(tuple1)..., get<T4::value>(tuple2)...);
}

constexpr auto tuple_cat(auto&&... tuple) {
    return (tuple + ... + Tuple<>());
}


template <size_t n = 0, typename... Args>
auto& operator>>(std::istream& is, Tuple<Args...>& tuple) {
    if constexpr (n < std::tuple_size_v<decltype(tuple)>) {
        is >> get<n>(tuple);
        operator>> <n+1>(is, tuple);
    }

    return is;
}

template<size_t n = 0, typename... Args>
auto& operator<<(std::ostream& os, Tuple<Args...>& tuple) {
    if constexpr (n < std::tuple_size_v<decltype(tuple)>) {
        os << get<n>(tuple);
        if constexpr (n + 1 < std::tuple_size_v<decltype(tuple)>) {
            os << ' ';
        }
        operator<< <n+1>(os, tuple);
    }

    return os;
}
