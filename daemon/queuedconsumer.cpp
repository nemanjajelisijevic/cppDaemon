#include "queuedconsumer.h"
#include "log.h"

namespace daemonize {

    QueuedConsumer::QueuedConsumer()
        : myServiceObject(new ServiceObject(nullptr))
    {
        myServiceObject->setName("Queued Consumer");
        _LOG_DEBUG_S(QString("[%1] Default Constructor").arg(QString::fromStdString(myServiceObject->getName())))
    }

    QueuedConsumer::QueuedConsumer(const QueuedConsumer& other)
        : myServiceObject(other.myServiceObject)
    {
        _LOG_DEBUG_S(QString("[%1] Copy Constructor").arg(QString::fromStdString(myServiceObject->getName())))
    }

    QueuedConsumer::QueuedConsumer(QueuedConsumer&& other)
        : myServiceObject(other.myServiceObject)
    {
        _LOG_DEBUG_S(QString("[%1] Move Constructor").arg(QString::fromStdString(myServiceObject->getName())))
    }

    QueuedConsumer::~QueuedConsumer()
    {
        _LOG_DEBUG_S(QString("[%1] Destructor").arg(QString::fromStdString(myServiceObject->getName())))
    }

    void QueuedConsumer::consume(IFunctor* callback)
    {
        myServiceObject->enqueue(callback);
    }

    void QueuedConsumer::setName(std::string&& name)
    {
        myServiceObject->setName(std::move(name));
    }

}
