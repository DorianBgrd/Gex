#ifndef GEX_PTRS_H
#define GEX_PTRS_H

#include <vector>
#include <memory>

#include "boost/weak_ptr.hpp"

#include <boost/python.hpp>

namespace Gex
{
    template<class T>
    class BaseWkPtr: public std::weak_ptr<T>
    {
    public:
        using std::weak_ptr<T>::weak_ptr;

        inline static std::shared_ptr<T> invalid = std::make_shared<T>();

        BaseWkPtr(const std::weak_ptr<T>& other):
            std::weak_ptr<T>(other) {}

        operator bool() const
        {
            return (!this->expired());
        }

        bool operator==(const BaseWkPtr<T>& other)
        {
            if (this->expired() || other.expired())
                return false;

            return ToShared().get() == other.ToShared().get();
        }

        std::shared_ptr<T> ToShared() const
        {
            if (!this->expired())
                return this->lock();
            return invalid;
        }

        std::shared_ptr<T> operator->() const
        {
            return ToShared();
        }
    };


    template<typename T>
    bool IsWkValid(const T& weakObj)
    {
        return (!weakObj.expired());
    }


    template<typename T>
    std::shared_ptr<T> ToSharedPtr(const std::weak_ptr<T>& weakObj)
    {
        if (!IsWkValid(weakObj))
            return {};

        return weakObj.lock();
    }


    template<typename T>
    bool AreWkEqual(const T& w1, const T& w2)
    {
        if (!IsWkValid(w1) || !IsWkValid(w2))
            return false;

        return (ToSharedPtr(w1) == ToSharedPtr(w2));
    }


    template<typename T>
    struct CheckWkPred
    {
        T cmp;

        CheckWkPred(T wk){cmp = wk;};

       bool operator()(const T& other)
       {
           return AreWkEqual(cmp, other);
       }
    };


    template<typename T>
    bool WkInVector(const std::vector<T>& wkVector, const T& wk)
    {
        return std::find_if(wkVector.begin(), wkVector.end(),
                            CheckWkPred(wk)) != wkVector.end();
    }


#define CALL_NO_ARG(obj, function, defaultR) \
    if (!IsValid(obj))                \
        return defaultR;              \
    return ToSharedPtr(obj)->function();


#define CALL_1ARG(obj, function, arg, defaultR) \
    if (!IsValid(obj))                \
        return defaultR;              \
    return ToSharedPtr(obj)->function(arg);

    template<class T>
    class WkList: public std::vector<std::weak_ptr<T>>
    {
    public:

    };

    template<class T>
    std::vector<std::weak_ptr<T>> ToWkVector(std::vector<std::shared_ptr<T>> sharedVector)
    {
        return {sharedVector.begin(), sharedVector.end()};
    }

    template<class T>
    std::vector<std::shared_ptr<T>> ToSharedVector(std::vector<std::weak_ptr<T>> weakVector)
    {
        std::vector<std::shared_ptr<T>> res;
        for (const auto& wkPtr : weakVector)
        {
            auto shared = ToSharedPtr(wkPtr);
            if (!shared)
                continue;

            res.push_back(shared);
        }

        return res;
    }

    namespace Python
    {
        template<class T>
        struct WeakPtrToPython
        {
            static PyObject* convert(Gex::BaseWkPtr<T> const& x)
            {
                return boost::python::converter::registered<T>::converters.to_python(
                        x.ToShared().get()
                );
            }

            static PyTypeObject const* get_pytype()
            {
                return boost::python::converter::registered<T>::converters.get_class_object();
            }
        };
    }
}

#endif //GEX_PTRS_H
