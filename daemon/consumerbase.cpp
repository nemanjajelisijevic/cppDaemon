#include "consumerbase.h"

namespace daemonize {

    Consumer::Consumer()
    {}

    Consumer::~Consumer()
    {}

    void Consumer::consume(IFunctor* callback)
    {
        callback->execute();
        delete callback;
    }

}
