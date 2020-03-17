#ifndef DAEMON_H
#define DAEMON_H

#include <memory>
#include "consumerbase.h"
#include "serviceobject.h"
#include "functorimpl.h"
#include <functional>

namespace daemonize {

    template<class ProtoType>
    class DaemonBase
    {
    public:

        virtual ~DaemonBase();

        DaemonBase<ProtoType>& setDefaultConsumer(Consumer* consumer);
        DaemonBase<ProtoType>& setName(std::string&& name);
        const std::string& getName() const; //TODO LDRA

        virtual ProtoType& getPrototype() const = 0;

    protected:

        DaemonBase() = delete;
        explicit DaemonBase(ServiceObject* prototype);
        DaemonBase(const DaemonBase<ProtoType>& other);
        DaemonBase(DaemonBase<ProtoType>&& other);


        DaemonBase<ProtoType>& operator=(const DaemonBase<ProtoType>& other) = delete;
        DaemonBase<ProtoType>& operator=(DaemonBase<ProtoType>&& other) = delete;

        template<typename Func, typename... Args>
        Functor<decltype(ret(Func()))>* makeFunctor(ProtoType* prototype, ServiceObject* serviceObject, Consumer* consumer, Func&& function, Args&&... args) const;

        std::shared_ptr<ServiceObject> myServiceObject;
    };

    template <typename ProtoType>
    inline DaemonBase<ProtoType>::DaemonBase(ServiceObject* serviceObject)
        : myServiceObject(serviceObject)
    {
        setName("DaemonBase");
    }

    template <typename ProtoType>
    inline DaemonBase<ProtoType>::~DaemonBase()
    {}

    template <typename ProtoType>
    inline DaemonBase<ProtoType>::DaemonBase(const DaemonBase<ProtoType>& other)
        : myServiceObject(other.myServiceObject)
    {}

    template <typename ProtoType>
    inline DaemonBase<ProtoType>::DaemonBase(DaemonBase<ProtoType>&& other)
        : myServiceObject(other.myServiceObject)
    {}

    template <typename ProtoType>
    inline DaemonBase<ProtoType>& DaemonBase<ProtoType>::setDefaultConsumer(Consumer* consumer)
    {
        myServiceObject->setDefaultConsumer(consumer);
        return *this;
    }

    template <typename ProtoType>
    inline DaemonBase<ProtoType>& DaemonBase<ProtoType>::setName(std::string&& name)
    {
        myServiceObject->setName(std::move(name));
        return *this;
    }

    template <typename ProtoType>
    inline const std::string& DaemonBase<ProtoType>::getName() const
    {
        return myServiceObject->getName();
    }

    template<class ProtoType>
    template<typename Func, typename... Args>
    inline Functor<decltype(ret(Func()))>* DaemonBase<ProtoType>::makeFunctor(
            ProtoType* prototype,
            ServiceObject* serviceObject,
            Consumer* consumer,
            Func&& function,
            Args&&... args) const
    {
        using ResultType = decltype(ret(Func()));
        return new FunctorImpl<ProtoType, ResultType, Func, Args...>(
                    prototype,
                    serviceObject,
                    consumer,
                    std::forward<Func>(function),
                    std::forward<Args>(args)...
                    );
    }

    template<class ProtoType>
    class Daemon : public DaemonBase<ProtoType>
    {
    public:

        Daemon() = delete;
        Daemon(Consumer* consumer, ProtoType* prototype);
        virtual ~Daemon() override;
        Daemon(const Daemon<ProtoType>& other);
        Daemon(Daemon<ProtoType>&& other);

        Daemon<ProtoType>& operator=(const Daemon<ProtoType>& other) = delete;
        Daemon<ProtoType>& operator=(Daemon<ProtoType>&& other) = delete;

        ProtoType& getPrototype() const override;

        template<typename Func, typename... Args>
        Functor<decltype(ret(Func()))>& async(Func&& function, Args&&... args) const;

        template<typename Func, typename... Args>
        Functor<decltype(ret(Func()))>& async(Consumer* consumer, Func&& function, Args&&... args) const;

    private:

        ProtoType* myPrototype;
    };

    template<class ProtoType>
    inline Daemon<ProtoType>::Daemon(Consumer* consumer, ProtoType* prototype)
        : DaemonBase<ProtoType> (new ServiceObject(consumer))
        , myPrototype(prototype)
    {
        DaemonBase<ProtoType>::setName("Daemon");
    }

    template<class ProtoType>
    inline Daemon<ProtoType>::~Daemon()
    {}

    template<class ProtoType>
    inline Daemon<ProtoType>::Daemon(const Daemon<ProtoType>& other)
        : DaemonBase<ProtoType> (other)
        , myPrototype(other.myPrototype)
    {}

    template<class ProtoType>
    inline Daemon<ProtoType>::Daemon(Daemon<ProtoType>&& other)
        : DaemonBase<ProtoType> (std::forward<DaemonBase<ProtoType>>(other))
        , myPrototype(other.myPrototype)
    {}

    template<class ProtoType>
    inline ProtoType& Daemon<ProtoType>::getPrototype() const
    {
        return *myPrototype;
    }

    template<class ProtoType>
    template<typename Func, typename... Args>
    inline Functor<decltype(ret(Func()))>& Daemon<ProtoType>::async(Func&& function, Args&&... args) const
    {
        return async(this->myServiceObject->getDefaultConsumer(), std::forward<Func>(function), std::forward<Args>(args)...);
    }

    template<class ProtoType>
    template<typename Func, typename... Args>
    inline Functor<decltype(ret(Func()))>& Daemon<ProtoType>::async(Consumer* consumer, Func&& function, Args&&... args) const
    {
        return *(DaemonBase<ProtoType>::makeFunctor(
                     this->myPrototype,
                     this->myServiceObject.get(),
                     consumer,
                     std::forward<Func>(function),
                     std::forward<Args>(args)...
                     ));
    }

    template<class ProtoType>
    class ManagedDaemon : public DaemonBase<ProtoType>
    {
    public:

        ManagedDaemon() = delete;
        ManagedDaemon(Consumer* consumer, ProtoType* prototype);
        ManagedDaemon(const ManagedDaemon<ProtoType>& other);
        ManagedDaemon(ManagedDaemon<ProtoType>&& other);

        virtual ~ManagedDaemon() override;

        ManagedDaemon<ProtoType>& operator=(const ManagedDaemon<ProtoType>& other) = delete;
        ManagedDaemon<ProtoType>& operator=(ManagedDaemon<ProtoType>&& other) = delete;

        ProtoType& getPrototype() const override;

        template<typename Func, typename... Args>
        Functor<decltype(ret(Func()))>& async(Func&& function, Args&&... args) const;

        template<typename Func, typename... Args>
        Functor<decltype(ret(Func()))>& async(Consumer* consumer, Func&& function, Args&&... args) const;

    private:

        std::shared_ptr<ProtoType> myPrototype;
    };

    template<class ProtoType>
    inline ManagedDaemon<ProtoType>::ManagedDaemon(Consumer* consumer, ProtoType* prototype)
        : DaemonBase<ProtoType> (new ServiceObject(consumer))
        , myPrototype(prototype)
    {
        DaemonBase<ProtoType>::setName("ManagedDaemon");
    }

    template<class ProtoType>
    inline ManagedDaemon<ProtoType>::~ManagedDaemon()
    {}

    template<class ProtoType>
    inline ManagedDaemon<ProtoType>::ManagedDaemon(const ManagedDaemon<ProtoType>& other)
        : DaemonBase<ProtoType> (other)
        , myPrototype(other.myPrototype)
    {}

    template<class ProtoType>
    inline ManagedDaemon<ProtoType>::ManagedDaemon(ManagedDaemon<ProtoType>&& other)
        : DaemonBase<ProtoType> (std::forward<DaemonBase<ProtoType>>(other))
        , myPrototype(other.myPrototype)
    {}

    template<class ProtoType>
    inline ProtoType& ManagedDaemon<ProtoType>::getPrototype() const
    {
        return *(myPrototype.get());
    }

    template<class ProtoType>
    template<typename Func, typename... Args>
    inline Functor<decltype(ret(Func()))>& ManagedDaemon<ProtoType>::async(Func&& function, Args&&... args) const
    {
        return async(this->myServiceObject->getDefaultConsumer(), std::forward<Func>(function), std::forward<Args>(args)...);
    }

    template<class ProtoType>
    template<typename Func, typename... Args>
    inline Functor<decltype(ret(Func()))>& ManagedDaemon<ProtoType>::async(Consumer* consumer, Func&& function, Args&&... args) const
    {
        return *(DaemonBase<ProtoType>::makeFunctor(myPrototype.get(), this->myServiceObject.get(), consumer, std::forward<Func>(function), std::forward<Args>(args)...));
    }
}


#endif // Daemon_H
