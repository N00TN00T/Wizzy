#pragma once

namespace Wizzy
{
    template <typename... Ts>
    struct largest_type;

    template <typename T>
    struct largest_type<T>
    {
        using type = T;
    };

    template <typename T, typename U, typename... Ts>
    struct largest_type<T, U, Ts...>
    {
        using type = typename largest_type<typename std::conditional<
                (sizeof(U) <= sizeof(T)), T, U
            >::type, Ts...
        >::type;
    };

    template<typename Target, typename ListHead, typename... ListTails>
    inline constexpr size_t index_of_type()
    {
        if constexpr (std::is_same<Target, ListHead>())
            return 0;
        else
            return 1 + index_of_type<Target, ListTails...>();
    }

    template<typename Target, typename ListHead, typename... ListTails>
    inline constexpr size_t index_of_first_convertible_type()
    {
        if constexpr (std::is_convertible<Target, ListHead>())
            return 0;
        else
            return 1 + index_of_first_convertible_type<Target, ListTails...>();
    }

    template<typename What, typename ... Args>
    struct is_present {
        static constexpr bool value {(std::is_same_v<What, Args> || ...)};
    };

    template<typename What, typename ... Args>
    struct is_convertible_present {
        static constexpr bool value {(std::is_convertible_v<What, Args> || ...)};
    };

    template <typename... Ts>
    struct first_convertible;

    template <typename T>
    struct first_convertible<T>
    {
        using type = T;
    };

    template <typename T, typename U, typename... Ts>
    struct first_convertible<T, U, Ts...>
    {
        using type = typename first_convertible<typename std::conditional<
                (std::is_convertible<U, T>()), U, T
            >::type, Ts...
        >::type;
    };



    namespace detail {
        template <class Default, class AlwaysVoid,
                template<class...> class Op, class... Args>
        struct detector
        {
            using value_t = std::false_type;
            using type = Default;
        };

        template <class Default, template<class...> class Op, class... Args>
        struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
        {
            using value_t = std::true_type;
            using type = Op<Args...>;
        };
    }

    struct nonesuch {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };


    template <template<class...> class Op, class... Args>
    using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

    template <template<class...> class Op, class... Args>
    using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

    template <class Default, template<class...> class Op, class... Args>
    using detected_or = detail::detector<Default, void, Op, Args...>;

    template<typename T, typename U>
    using comparable_t = decltype(std::declval<T&>() == std::declval<const U&>());

    template<typename T, typename U>
    using is_comparable = is_detected<comparable_t, T, U>;

    template<typename T>
    using is_self_comparable = is_comparable<T, T>;


    template<typename T, typename U>
    using formattable_t = decltype(std::declval<T&>() << std::declval<const U&>());

    template<typename T, typename U>
    using is_formattable = is_detected<comparable_t, T, U>;


    template<class T, class EqualTo>
    struct has_operator_equal_impl
    {
        template<class U, class V>
        static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
        template<typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
    };

    template<class T, class EqualTo = T>
    struct has_operator_equal : has_operator_equal_impl<T, EqualTo>::type {};

    template<class T, class Target>
    struct has_format_impl
    {
        template<class U, class V>
        static auto test(U*) -> decltype(std::declval<V>() << std::declval<U>());
        template<typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, Target>(0))>::type;
    };

    template<class T, class Target>
    struct has_format : has_format_impl<T, Target>::type {};
    
    template<class X, class Y, class Op>
    struct op_valid_impl
    {
        template<class U, class L, class R>
        static auto test(int) -> decltype(std::declval<U>()(std::declval<L>(), std::declval<R>()),
                                          void(), std::true_type());

        template<class U, class L, class R>
        static auto test(...) -> std::false_type;

        using type = decltype(test<Op, X, Y>(0));

    };

    template<class X, class Y, class Op> using op_valid = typename op_valid_impl<X, Y, Op>::type;

    namespace notstd {

        struct left_shift {

            template <class L, class R>
            constexpr auto operator()(L&& l, R&& r) const
            noexcept(noexcept(std::forward<L>(l) << std::forward<R>(r)))
            -> decltype(std::forward<L>(l) << std::forward<R>(r))
            {
                return std::forward<L>(l) << std::forward<R>(r);
            }
        };

        struct right_shift {

            template <class L, class R>
            constexpr auto operator()(L&& l, R&& r) const
            noexcept(noexcept(std::forward<L>(l) >> std::forward<R>(r)))
            -> decltype(std::forward<L>(l) >> std::forward<R>(r))
            {
                return std::forward<L>(l) >> std::forward<R>(r);
            }
        };

    }

    template<class X, class Y> using has_equality = op_valid<X, Y, std::equal_to<>>;
    /*template<class X, class Y> using has_inequality = op_valid<X, Y, std::not_equal_to<>>;
    template<class X, class Y> using has_less_than = op_valid<X, Y, std::less<>>;
    template<class X, class Y> using has_less_equal = op_valid<X, Y, std::less_equal<>>;
    template<class X, class Y> using has_greater_than = op_valid<X, Y, std::greater<>>;
    template<class X, class Y> using has_greater_equal = op_valid<X, Y, std::greater_equal<>>;
    template<class X, class Y> using has_bit_xor = op_valid<X, Y, std::bit_xor<>>;
    template<class X, class Y> using has_bit_or = op_valid<X, Y, std::bit_or<>>;
    template<class X, class Y> using has_left_shift = op_valid<X, Y, notstd::left_shift>;
    template<class X, class Y> using has_right_shift = op_valid<X, Y, notstd::right_shift>;*/


    /*template<typename, typename = void>
    inline constexpr bool has_left_shift = std::false_type{};

    template<typename TStream, typename TUserType>
    inline constexpr bool has_left_shift<
        TUserType,
        std::void_t<
            decltype(
                std::declval<TStream&>() << std::declval<TUserType>()
            )
        >
    > = std::true_type{};*/

    template<typename, typename = void>
    inline constexpr bool has_leftshift_ostream = std::false_type{};

    template<typename T>
    inline constexpr bool has_leftshift_ostream<
        T,
        std::void_t<
            decltype(
                std::declval<std::ostream&>() << std::declval<T>()
            )
        >
    > = std::true_type{};


    template<typename, typename = void>
    inline constexpr bool has_rightshift_istream = std::false_type{};

    template<typename T>
    inline constexpr bool has_rightshift_istream<
        T,
        std::void_t<
            decltype(
                std::declval<std::istream&>() >> std::declval<T>()
            )
        >
    > = std::true_type{};


    template<typename, typename = void>
    inline constexpr bool has_leftshift_stringstream = std::false_type{};

    template<typename T>
    inline constexpr bool has_leftshift_stringstream<
        T,
        std::void_t<
            decltype(
                std::declval<std::stringstream&>() << std::declval<T>()
            )
        >
    > = std::true_type{};

    template<typename, typename = void>
    inline constexpr bool has_rightshift_stringstream = std::false_type{};

    template<typename T>
    inline constexpr bool has_rightshift_stringstream<
        T,
        std::void_t<
            decltype(
                std::declval<std::stringstream&>() >> std::declval<T>()
            )
        >
    > = std::true_type{};
}