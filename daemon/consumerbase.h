#ifndef CONSUMERBASE_H
#define CONSUMERBASE_H

#include "functorbase.h"
#include <string>

namespace daemonize {

    class Consumer
    {
    public:
        Consumer();
        Consumer(const Consumer& other) = delete;
        Consumer(Consumer&& other) = delete;

        Consumer& operator=(const Consumer& other) = delete;
        Consumer& operator=(Consumer&& other) = delete;

        virtual ~Consumer();
        virtual void consume(IFunctor* callback);
        virtual void setName(std::string&& name) = 0;
    };


}
#endif // CONSUMERBASE_H
