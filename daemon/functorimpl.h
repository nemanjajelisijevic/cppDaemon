#ifndef FUNCTORIMPL_H
#define FUNCTORIMPL_H

#include "functorbase.h"
#include "returningclosure.h"
#include "voidclosure.h"
#include "consumerbase.h"
#include "serviceobject.h"
#include <functional>

namespace daemonize {

    namespace template_arg_helper
    {
        template <int... Is>
        struct index {};

        template <int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};//TODO LDRA

        template <int... Is>
        struct gen_seq<0, Is...> : index<Is...> {};
    }

    template<typename Class, typename ResultType, typename... Args>
    ResultType ret(ResultType(Class::*)(Args...));

    template<typename ResultType>
    class Functor : public IFunctor
    {
    public:
        Functor();

        Functor(const Functor& other) = delete;
        Functor(Functor&& other) = delete;

        Functor& operator=(const Functor& other) = delete;
        Functor& operator=(Functor&& other) = delete;

        virtual ~Functor() override;
        virtual void setClosure(std::function<void(ResultType)>&& closure) = 0;
        virtual void setRecursiveClosure(std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)>&& closure) = 0;
    };

    template<typename ResultType>
    inline Functor<ResultType>::Functor()
    {}

    template<typename ResultType>
    inline Functor<ResultType>::~Functor()
    {}

    template<>
    class Functor<void> : public IFunctor
    {
    public:
        virtual ~Functor(){}
        virtual void setClosure(std::function<void()>&& closure) = 0;
        virtual void setRecursiveClosure(std::function<void(RecursiveVoidClosureWrapper&)>&& closure) = 0;
        virtual void noClosure() = 0;
    };

    template<class ProtoType, typename ResultType, typename Function, typename... Args>
    class FunctorImpl : public Functor<ResultType>
    {
    public:

        FunctorImpl() = delete;
        FunctorImpl(ProtoType* prototype, ServiceObject* object, Consumer* consumer, Function&& func, Args&&... arguments);

        FunctorImpl(const FunctorImpl& other) = delete;
        FunctorImpl(FunctorImpl&& other) = delete;

        FunctorImpl& operator=(const FunctorImpl& other) = delete;
        FunctorImpl& operator=(FunctorImpl&& other) = delete;

        virtual ~FunctorImpl() override;
        void setClosure(std::function<void(ResultType)>&& closure) override;
        void setRecursiveClosure(std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)>&& closure);
        void execute() override;

    private:

        ProtoType* myPrototype;
        ServiceObject* serviceObject;
        Consumer* myConsumer;

        bool recursiveClosure;
        std::function<void(ResultType)> myClosure;
        std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)> myRecursiveClosure;

        std::function<ResultType(Args...)> myMethod;
        std::tuple<Args...> args;

        template <int... Is>
        ResultType funct(std::tuple<Args...>& tup, template_arg_helper::index<Is...>);
        ResultType func(std::tuple<Args...>& tup);
    };

    template<class ProtoType, typename ResultType,  typename Function, typename... Args>
    inline FunctorImpl<ProtoType, ResultType, Function, Args...>::FunctorImpl(
            ProtoType* prototype,
            ServiceObject* object,
            Consumer* consumer,
            Function&& func,
            Args&&... arguments
            )
        : myPrototype(prototype)
        , serviceObject(object)
        , myConsumer(consumer)
        , recursiveClosure{false}
        , myClosure{}
        , myRecursiveClosure{}
        , myMethod{std::bind(std::forward<Function>(func), prototype, std::forward<Args>(arguments)...)}
        , args{std::forward<Args>(arguments)...}
    {}

    template<class ProtoType, typename ResultType,  typename Function, typename... Args>
    inline FunctorImpl<ProtoType, ResultType, Function, Args...>::~FunctorImpl()
    {}

    template<class ProtoType, typename ResultType,  typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, ResultType,  Function, Args...>::setClosure(std::function<void(ResultType)>&& closure)
    {
        myClosure = std::forward<std::function<void(ResultType)>>(closure);
        serviceObject->enqueue(this);
    }

    template<class ProtoType, typename ResultType,  typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, ResultType,  Function, Args...>::setRecursiveClosure(std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)>&& closure)
    {
        recursiveClosure = true;
        myRecursiveClosure = std::forward<std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)>>(closure);
        serviceObject->enqueue(this);
    }

    template<class ProtoType, typename ResultType, typename Function, typename... Args>
    template<int... Is>
    inline ResultType FunctorImpl<ProtoType, ResultType, Function, Args...>::funct(std::tuple<Args...>& tup, template_arg_helper::index<Is...>)
    {
        return myMethod(std::get<Is>(tup)...);
    }

    template<class ProtoType, typename ResultType, typename Function, typename... Args>
    inline ResultType FunctorImpl<ProtoType, ResultType, Function, Args...>::func(std::tuple<Args...>& tup)
    {
        return funct(tup, template_arg_helper::gen_seq<sizeof...(Args)>{});
    }

    template<class ProtoType, typename ResultType, typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, ResultType, Function, Args...>::execute()
    {
        ResultType result = func(args);
        if(serviceObject->stopped() == false)
        {
            IFunctor* closure{nullptr};

            if (!recursiveClosure)
            {
                closure = new ReturningClosureWrapper<ResultType>(
                            std::forward<std::function<void(ResultType)>>(myClosure),
                            std::forward<ResultType>(result));
            }
            else
            {
                closure = new RecursiveClosureWrapper<ResultType>(
                            std::forward<ResultType>(result),
                            std::forward<std::function<void(ResultType, RecursiveClosureWrapper<ResultType>&)>>(myRecursiveClosure));
            }

            myConsumer->consume(closure);
        }
    }

    template<typename ProtoType, typename Function, typename... Args>
    class FunctorImpl<ProtoType, void, Function, Args...> : public Functor<void>
    {
    public:

        FunctorImpl(ProtoType* prototype, ServiceObject* object, Consumer* consumer, Function&& function, Args&&... arguments);
        virtual ~FunctorImpl() override;

        void setClosure(std::function<void(void)>&& closure) override;
        void setRecursiveClosure(std::function<void(RecursiveVoidClosureWrapper&)>&& closure) override;
        void noClosure() override;
        void execute() override;

    private:

        ProtoType* myPrototype;
        ServiceObject* serviceObject;
        Consumer* myConsumer;

        bool consumeClosure;
        bool recursiveClosure;
        std::function<void(void)> myClosure;
        std::function<void(RecursiveVoidClosureWrapper&)> myRecursiveClosure;

        std::function<void(Args...)> myMethod;
        std::tuple<Args...> args;

        template <int... Is> //TODO LDRA
        void funct(std::tuple<Args...>& tup, template_arg_helper::index<Is...>);
        void func(std::tuple<Args...>& tup);
    };

    template<class ProtoType, typename Function, typename... Args>
    inline FunctorImpl<ProtoType, void, Function, Args...>::FunctorImpl(
            ProtoType* prototype,
            ServiceObject* object,
            Consumer* consumer,
            Function&& function,
            Args&&... arguments
            )
        : myPrototype(prototype)
        , serviceObject(object)
        , myConsumer(consumer)
        , consumeClosure{false}
        , recursiveClosure{false}
        , myClosure{}
        , myRecursiveClosure{}
        , myMethod{std::bind(function, prototype, std::forward<Args&&>(arguments)...)}
        , args{std::forward<Args>(arguments)...}
    {}

    template<class ProtoType, typename Function, typename... Args>
    inline FunctorImpl<ProtoType, void, Function, Args...>::~FunctorImpl()
    {}

    template<class ProtoType, typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::setClosure(std::function<void(void)>&& closure)
    {
        consumeClosure = true;
        myClosure = std::forward<std::function<void(void)>>(closure);
        serviceObject->enqueue(this);
    }

    template<class ProtoType, typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::setRecursiveClosure(std::function<void(RecursiveVoidClosureWrapper&)>&& closure)
    {
        consumeClosure = true;
        recursiveClosure = true;
        myRecursiveClosure = std::forward<std::function<void(RecursiveVoidClosureWrapper&)>>(closure);
        serviceObject->enqueue(this);
    }

    template<class ProtoType, typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::noClosure()
    {
        consumeClosure = false;
        serviceObject->enqueue(this);
    }

    template<class ProtoType,  typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::execute()
    {
        func(args);
        if(!serviceObject->stopped() == false && !consumeClosure)
        {
            IFunctor* closure{nullptr};

            if (!recursiveClosure)
            {
                closure = new VoidClosureWrapper(std::forward<std::function<void()>>(myClosure));
            }
            else
            {
                closure = new RecursiveVoidClosureWrapper(std::forward<std::function<void(RecursiveVoidClosureWrapper&)>>(myRecursiveClosure));
            }

            myConsumer->consume(closure);
        }
    }

    template<class ProtoType, typename Function, typename... Args>
    template<int... Is>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::funct(std::tuple<Args...>& tup, template_arg_helper::index<Is...>)
    {
        myMethod(std::get<Is>(tup)...);
    }

    template<class ProtoType, typename Function, typename... Args>
    inline void FunctorImpl<ProtoType, void, Function, Args...>::func(std::tuple<Args...>& tup)
    {
       funct(tup, template_arg_helper::gen_seq<sizeof...(Args)>{});
    }

}

#endif // FUNCTORIMPL_H
