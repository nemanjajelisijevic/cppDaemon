#ifndef SERVICEOBJECT_H
#define SERVICEOBJECT_H

#include <thread>
#include "syncqueue.h"

namespace daemonize {

    class IFunctor;
    class Consumer;

    class ServiceObject
    {
    public:

        ServiceObject() = delete;
        explicit ServiceObject(Consumer* defaultConsumer);
        ~ServiceObject();

        ServiceObject(const ServiceObject& other) = delete;
        ServiceObject(ServiceObject&& other) = delete;

        ServiceObject& operator=(const ServiceObject& other) = delete;
        ServiceObject& operator=(ServiceObject&& other) = delete;

        void enqueue(IFunctor* functor);
        void setName(std::string&& name);
        const std::string& getName();

        bool stopped();
        ServiceObject& setDefaultConsumer(Consumer* consumer);
        Consumer* getDefaultConsumer();

    private:

        Consumer* myDefaultConsumer;
        SyncQueue<IFunctor*> syncStruct;

        std::thread myThread;
        std::string myName;
    };
}


#endif // SERVICEOBJECT_H
