//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_CORE_H
#define FREEGL_CORE_H

#if defined(ANDROID) || defined(__ANDROID__)
#include <bits/shared_ptr.h>

namespace freeze{

    template<typename...>
    struct always_true{constexpr static bool value = true;};

    template<typename... Xs>
    constexpr bool is_valid = always_true<Xs...>::value;

    template<typename Object>
    auto make = [](auto&&... xs){
        if constexpr (is_valid<decltype(xs)...>)
        {
            return Object{ static_cast<decltype(xs)&&>(xs)...};
        }
        else
        {
            static_assert((sizeof...(xs),false));
        }
    };
}

#endif //defined(ANDROID) || defined(__ANDROID__)

namespace freeze{

    template<typename T,bool NeedArgs = true>
    struct make_object
    {
        using targer_type = T;
        using target_pointer = T*;

        make_object()
        {
            auto pT = static_cast<target_pointer>(this);
            if constexpr(NeedArgs)
            {
                object_name = std::make_shared<GLuint>(0);
                pT->create(object_name.get());
            }
            else
            {
                object_name = std::make_shared<GLuint>(pT->create());
            }
            assert(object_name);
        }

        make_object(make_object const& rhs)
            : object_name{rhs.object_name}
        {

        }

        make_object(make_object&& rhs)
        {
            object_name = std::move(rhs.object_name);
            //rhs.object_name.reset();
        }

        make_object& operator=(make_object const& rhs) 
        {
            object_name = rhs.object_name;
            return *this;
        }

        make_object& operator=(make_object&& rhs) 
        {
            object_name = std::move(rhs.object_name);
            //rhs.object_name.reset();
            return *this;
        }

        ~make_object()
        {
            if(object_name && object_name.use_count()==1)
            {
                auto pT = static_cast<target_pointer>(this);
                if constexpr(NeedArgs)
                {
                    pT->destroy(object_name.get());
                }
                else
                {
                    pT->destroy(ref());
                }
            }
        }

        auto ref() const
        {
            return *object_name;
        }

        operator GLuint() const
        {
            return ref();
        }

        explicit operator bool() const
        {
            return *object_name > 0;
        }

    protected:
        std::shared_ptr<GLuint> object_name;
    };
}

namespace freeze 
{
    template<std::size_t N,typename... Vec>
    struct data_length;

    template<std::size_t N,typename V, typename... Vec>
    struct data_length<N, V, Vec...>
            : std::integral_constant<std::size_t,
                    data_length<N, V>::value + data_length<N, Vec...>::value>
    {

    };

    template<std::size_t N,typename V>
    struct data_length<N, V>
            : std::integral_constant<std::size_t,sizeof(V)*N>
    {

    };
}


#endif //FREEGL_CORE_H
