#ifndef SIMPLEPARSER_METHOD_H
#define SIMPLEPARSER_METHOD_H

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>

#include "Variant.h"
#include "Type.h"

namespace SimpleParser
{
    namespace Detail
    {
#if defined(__cpp_lib_apply) && __cpp_lib_apply > 201603L
		using std::apply;
#else
        // Fallback für C++14 und älter
        template<typename F, typename Tuple, std::size_t... I>
        decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
        {
            return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
        }

        template<typename F, typename Tuple>
        decltype(auto) apply(F&& f, Tuple&& t)
        {
            return apply_impl(
                std::forward<F>(f),
                std::forward<Tuple>(t),
                std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
            );
        }
#endif

        template<typename T>
        using DecayNoRef = std::remove_cv_t<std::remove_reference_t<T>>;

        template<typename T>
        T variant_cast(Variant& v)
        {
            using U = DecayNoRef<T>;

            if constexpr (std::is_same_v<U, Variant>)
                return v;
            else if constexpr (std::is_same_v<U, Variant*>)
                return static_cast<Variant*>(v);
            else
                return static_cast<U>(v);
        }

        template<typename T>
        Variant to_variant(T&& value)
        {
            using U = DecayNoRef<T>;

            if constexpr (std::is_same_v<U, Variant>)
                return std::forward<T>(value);
            else
                return Variant(std::forward<T>(value));
        }

        inline Variant to_variant_void()
        {
            return Variant();
        }

        template<typename... Args, std::size_t... I>
        std::tuple<Args...> build_args_tuple_impl(Variant** vals, std::index_sequence<I...>)
        {
            return std::tuple<Args...>{ variant_cast<Args>(*vals[I])... };
        }

        template<typename... Args>
        std::tuple<Args...> build_args_tuple(Variant** vals)
        {
            return build_args_tuple_impl<Args...>(vals, std::index_sequence_for<Args...>{});
        }
    }

    class Method
    {
    public:
        virtual ~Method() = default;

        virtual Variant execute(Variant* v)
        {
            return execute(v, nullptr, 0);
        }

        virtual Variant execute(Variant* v, Variant** vals, int argc) = 0;

        template<class O, class R, class... Args>
        static Method* bindMethod(R(O::* method)(Args...));

        template<class O, class R, class... Args>
        static Method* bindMethod(R(O::* method)(Args...) const);

    protected:
        std::string m_name;
    };

    template<class O, class R, class... Args>
    class MethodMember : public Method
    {
    public:
        using MethodPtr = R(O::*)(Args...);

        explicit MethodMember(MethodPtr method) : m_method(method) {}

        Variant execute(Variant* v, Variant** vals, int argc) override
        {
            if (!v || !m_method || argc != static_cast<int>(sizeof...(Args)))
                return Variant();

            O* obj = static_cast<O*>(v);

            auto args = Detail::build_args_tuple<Args...>(vals);

            if constexpr (std::is_void_v<R>) {
                std::apply(
                    [&](auto&&... unpacked) {
                        (obj->*m_method)(std::forward<decltype(unpacked)>(unpacked)...);
                    },
                    args
                );
                return Detail::to_variant_void();
            }
            else {
                R result = std::apply(
                    [&](auto&&... unpacked) -> R {
                        return (obj->*m_method)(std::forward<decltype(unpacked)>(unpacked)...);
                    },
                    args
                );
                return Detail::to_variant(std::move(result));
            }
        }

    private:
        MethodPtr m_method = nullptr;
    };

    template<class O, class R, class... Args>
    class MethodConstMember : public Method
    {
    public:
        using MethodPtr = R(O::*)(Args...) const;

        explicit MethodConstMember(MethodPtr method) : m_method(method) {}

        Variant execute(Variant* v, Variant** vals, int argc) override
        {
            if (!v || !m_method || argc != static_cast<int>(sizeof...(Args)))
                return Variant();

            const O* obj = static_cast<const O*>(v);

            auto args = Detail::build_args_tuple<Args...>(vals);

            if constexpr (std::is_void_v<R>) {
                std::apply(
                    [&](auto&&... unpacked) {
                        (obj->*m_method)(std::forward<decltype(unpacked)>(unpacked)...);
                    },
                    args
                );
                return Detail::to_variant_void();
            }
            else {
                R result = std::apply(
                    [&](auto&&... unpacked) -> R {
                        return (obj->*m_method)(std::forward<decltype(unpacked)>(unpacked)...);
                    },
                    args
                );
                return Detail::to_variant(std::move(result));
            }
        }

    private:
        MethodPtr m_method = nullptr;
    };

    template<class O, class R, class... Args>
    Method* Method::bindMethod(R(O::* method)(Args...))
    {
        return new MethodMember<O, R, Args...>(method);
    }

    template<class O, class R, class... Args>
    Method* Method::bindMethod(R(O::* method)(Args...) const)
    {
        return new MethodConstMember<O, R, Args...>(method);
    }
}

namespace SimpleParser
{
    template<class M>
    void Type::registerMethod(std::string name, M method)
    {
        auto mth = Method::bindMethod(method);
        registerMethod(name, mth);
    }
}

#endif // SIMPLEPARSER_METHOD_H