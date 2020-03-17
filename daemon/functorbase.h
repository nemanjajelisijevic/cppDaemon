#ifndef FUNCTOR_BASE_H
#define FUNCTOR_BASE_H

namespace daemonize {

    class IFunctor
    {
    public:
        IFunctor();

        virtual ~IFunctor();
        virtual void execute() = 0;

    private:

        IFunctor(const IFunctor& other) = delete;
        IFunctor(IFunctor&& other) = delete;

        IFunctor& operator=(const IFunctor& other) = delete;
        IFunctor& operator=(IFunctor&& other) = delete;
    };
}
#endif // FUNCTOR_BASE_H
