#include "voidclosure.h"

namespace daemonize {

    VoidClosureWrapper::VoidClosureWrapper(std::function<void(void)>&& func)
        : myCallback(std::forward<std::function<void(void)>>(func))
    {}

    VoidClosureWrapper::~VoidClosureWrapper()
    {}

    void VoidClosureWrapper::execute()
    {
        myCallback();
    }

    RecursiveVoidClosureWrapper::RecursiveVoidClosureWrapper(std::function<void(RecursiveVoidClosureWrapper&)>&& func)
        : myCallback(std::forward<std::function<void(RecursiveVoidClosureWrapper&)>>(func))
    {}

    RecursiveVoidClosureWrapper::RecursiveVoidClosureWrapper(const RecursiveVoidClosureWrapper& other)
        : myCallback(other.myCallback)
    {}

    RecursiveVoidClosureWrapper::RecursiveVoidClosureWrapper(RecursiveVoidClosureWrapper&& other)
        : myCallback(std::move(other.myCallback))
    {}

    RecursiveVoidClosureWrapper::~RecursiveVoidClosureWrapper()
    {}

    std::function<void(RecursiveVoidClosureWrapper&)>&& RecursiveVoidClosureWrapper::getClosure()
    {
        return std::move(myCallback);
    }

    void RecursiveVoidClosureWrapper::execute()
    {
        myCallback(*this);
    }
}
