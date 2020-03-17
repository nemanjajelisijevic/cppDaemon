#ifndef VOIDCLOSURE_H
#define VOIDCLOSURE_H

#include "functorbase.h"
#include <functional>

namespace daemonize {

    class VoidClosureWrapper : public IFunctor
    {
    public:

        VoidClosureWrapper() = delete;
        explicit VoidClosureWrapper(std::function<void(void)>&& func);

        VoidClosureWrapper(const VoidClosureWrapper& other) = delete;
        VoidClosureWrapper(VoidClosureWrapper&& other) = delete;

        VoidClosureWrapper& operator=(const VoidClosureWrapper& other) = delete;
        VoidClosureWrapper& operator=(VoidClosureWrapper&& other) = delete;

        virtual ~VoidClosureWrapper() override;
        void execute() override;

    private:

        std::function<void()> myCallback;
    };

    class RecursiveVoidClosureWrapper : public IFunctor
    {
    public:

        RecursiveVoidClosureWrapper() = delete;
        explicit RecursiveVoidClosureWrapper(std::function<void(RecursiveVoidClosureWrapper&)>&& func);

        RecursiveVoidClosureWrapper(const RecursiveVoidClosureWrapper& other);
        RecursiveVoidClosureWrapper(RecursiveVoidClosureWrapper&& other);

        RecursiveVoidClosureWrapper& operator=(const RecursiveVoidClosureWrapper& other) = delete;
        RecursiveVoidClosureWrapper& operator=(RecursiveVoidClosureWrapper&& other) = delete;

        virtual ~RecursiveVoidClosureWrapper() override;
        std::function<void(RecursiveVoidClosureWrapper&)>&& getClosure();
        void execute() override;

    private:

        std::function<void(RecursiveVoidClosureWrapper&)> myCallback;
    };

}

#endif // VOIDCLOSURE_H
