#include "serviceobject.h"
#include "consumerbase.h"
#include "functorbase.h"
#include "log.h"

namespace daemonize {

    ServiceObject::ServiceObject(Consumer* defaultConsumer)
        : myDefaultConsumer(defaultConsumer)
        , syncStruct{}
        , myName("Service Object")
    {
        _LOG_DEBUG_S(QString("[%1] Default constructor").arg(QString::fromStdString(myName)))
        auto sync = &syncStruct;
        myThread = std::thread(
        [sync] ()
        {
            while (sync->stopped() == false)
            {
                IFunctor* nextFunctor = sync->dequeue();
                if (nextFunctor != nullptr)
                {
                    nextFunctor->execute();
                    delete nextFunctor;
                }
            }

            //DEBUG
            //std::cout << "[" << getName() << "] Service thread going out of scope." << std::endl;
            //_LOG_DEBUG_S(QString("[ServiceObject] Service thread going out of scope."))
        });
    }

    ServiceObject::~ServiceObject()
    {
        _LOG_DEBUG_S(QString("[%1] Service Object Destructor").arg(QString::fromStdString(myName)))
        syncStruct.cleanUp();
        if (std::this_thread::get_id() != myThread.get_id())
        {
            myThread.join();
        }
    }

    bool ServiceObject::stopped()
    {
        return syncStruct.stopped();
    }

    ServiceObject& ServiceObject::setDefaultConsumer(Consumer* consumer)
    {
        myDefaultConsumer = consumer;
        return *this;
    }

    Consumer* ServiceObject::getDefaultConsumer()
    {
        return myDefaultConsumer;
    }

    void ServiceObject::enqueue(IFunctor *functor)
    {
        syncStruct.enqueue(functor);
    }

    void ServiceObject::setName(std::string&& name)
    {
        myName = std::move(name);
    }

    const std::string& ServiceObject::getName()
    {
        return myName;
    }

}
