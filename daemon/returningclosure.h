#ifndef RETURNINGCLOSURE_H
#define RETURNINGCLOSURE_H

#include "functorbase.h"
#include <functional>

namespace daemonize {

    template<typename R>
    class ReturningClosureWrapper : public IFunctor
    {
    public:

        ReturningClosureWrapper() = delete;
        template <typename Res>
        ReturningClosureWrapper(std::function<void(R)>&& func, Res&& result);

        ReturningClosureWrapper(const ReturningClosureWrapper& other) = delete;
        ReturningClosureWrapper(ReturningClosureWrapper&& other) = delete;

        ReturningClosureWrapper& operator=(const ReturningClosureWrapper& other) = delete;
        ReturningClosureWrapper& operator=(ReturningClosureWrapper&& other) = delete;

        virtual ~ReturningClosureWrapper() override;
        void execute() override;

    private:

        std::function<void(R)> myCallback;
        R myResult;
    };

    template<typename R>
    inline ReturningClosureWrapper<R>::~ReturningClosureWrapper()
    {}

    template<typename R>
    template <typename Res>
    inline ReturningClosureWrapper<R>::ReturningClosureWrapper(std::function<void(R)>&& func, Res&& result)
        : myCallback(std::forward<std::function<void(R)>>(func))
        , myResult(std::forward<Res>(result))
    {}

    template<typename R>
    inline void ReturningClosureWrapper<R>::execute()
    {
        myCallback(myResult);
    }

    template<typename R>
    class RecursiveClosureWrapper : public IFunctor
    {
    public:

        RecursiveClosureWrapper() = delete;
        template <typename Res>
        RecursiveClosureWrapper(Res&& result, std::function<void(R, RecursiveClosureWrapper<R>&)>&& func);

        RecursiveClosureWrapper(const RecursiveClosureWrapper& other);
        RecursiveClosureWrapper(RecursiveClosureWrapper&& other);

        RecursiveClosureWrapper& operator=(const RecursiveClosureWrapper& other) = delete;
        RecursiveClosureWrapper& operator=(RecursiveClosureWrapper&& other) = delete;

        virtual ~RecursiveClosureWrapper() override;
        void execute() override;
        std::function<void(R, RecursiveClosureWrapper<R>&)>&& getClosure();//TODO LDRA

    private:

        R myResult;
        std::function<void(R, RecursiveClosureWrapper<R>&)> myCallback;
    };

    template<typename R>
    inline RecursiveClosureWrapper<R>::~RecursiveClosureWrapper()
    {}

    template<typename R>
    template <typename Res>
    inline RecursiveClosureWrapper<R>::RecursiveClosureWrapper(Res&& result, std::function<void(R, RecursiveClosureWrapper<R>&)>&& func)
        : myResult(std::forward<Res>(result))
        , myCallback(std::forward<std::function<void(R, RecursiveClosureWrapper<R>&)>>(func))
    {}

    template<typename R>
    inline RecursiveClosureWrapper<R>::RecursiveClosureWrapper(const RecursiveClosureWrapper& other)
        : myResult(other.myResult)
        , myCallback(other.myCallback)
    {}

    template<typename R>
    inline RecursiveClosureWrapper<R>::RecursiveClosureWrapper(RecursiveClosureWrapper&& other)
        : myResult(std::forward<R>(other.myResult))
        , myCallback(std::forward<std::function<void(R, RecursiveClosureWrapper<R>&)>>(other.myCallback))
    {}

    template<typename R>
    inline void RecursiveClosureWrapper<R>::execute()
    {
        myCallback(myResult, *this);
    }

    template<typename R>
    inline std::function<void(R, RecursiveClosureWrapper<R>&)>&& RecursiveClosureWrapper<R>::getClosure()
    {
        return std::forward<std::function<void(R, RecursiveClosureWrapper<R>&)>>(myCallback);
    }
}


#endif // RETURNINGCLOSURE_H
