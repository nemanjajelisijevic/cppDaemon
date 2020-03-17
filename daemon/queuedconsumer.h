#ifndef QUEUEDCONSUMER_H
#define QUEUEDCONSUMER_H

#include <memory>
#include "serviceobject.h"
#include "consumerbase.h"

namespace daemonize {

    class QueuedConsumer : public Consumer
    {
    public:

        QueuedConsumer();

        QueuedConsumer(const QueuedConsumer& other);
        QueuedConsumer(QueuedConsumer&& other);

        QueuedConsumer& operator=(const QueuedConsumer& other) = delete;
        QueuedConsumer& operator=(QueuedConsumer&& other) = delete;

        virtual ~QueuedConsumer() override;

        void consume(IFunctor* callback) override;
        void setName(std::string&& name) override;

    private:

        std::shared_ptr<ServiceObject> myServiceObject;
    };
}


#endif // QUEUEDCONSUMER_H
