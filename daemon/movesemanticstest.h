#ifndef MOVESEMANTICSTEST_H
#define MOVESEMANTICSTEST_H

#include <vector>
#include <functional>
#include <thread>
#include "log.h"

namespace daemonize {

    template<typename T>
    class MoveSemanticsTest
    {
    public:
        MoveSemanticsTest();
        MoveSemanticsTest(const MoveSemanticsTest& other);
        MoveSemanticsTest(MoveSemanticsTest&& other);
        ~MoveSemanticsTest();

        MoveSemanticsTest& setSleepTime(std::chrono::seconds sec);
        void add(T element);
        MoveSemanticsTest<T>& addd(T element);
        std::vector<T>& getElements();

        void forEach(std::function<void(T&)>&& action);
        int getNumber();

    private:
        std::vector<T> myVector;
        std::chrono::seconds sleepTime;
    };

    template<typename T>
    inline MoveSemanticsTest<T>::MoveSemanticsTest()
        : myVector()
        , sleepTime(0)
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Default constructor")
    }


    template<typename T>
    inline MoveSemanticsTest<T>::MoveSemanticsTest(const MoveSemanticsTest<T>& other)
        : myVector()
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Copy constructor")
        for(auto val : other.myVector)
        {
            myVector.push_back(val);
        }
    }

    template<typename T>
    inline MoveSemanticsTest<T>::MoveSemanticsTest(MoveSemanticsTest<T>&& other)
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Move constructor")
        std::swap(myVector, other.myVector);
    }

    template<typename T>
    inline MoveSemanticsTest<T>::~MoveSemanticsTest()
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Destructor")
    }

    template<typename T>
    inline MoveSemanticsTest<T>& MoveSemanticsTest<T>::setSleepTime(std::chrono::seconds sec)
    {
        sleepTime = sec;
        return *this;
    }

    template<typename T>
    inline void MoveSemanticsTest<T>::add(T element)
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Adding.....")
        std::this_thread::sleep_for(sleepTime);
        myVector.push_back(element);
    }

    template<typename T>
    inline MoveSemanticsTest<T>& MoveSemanticsTest<T>::addd(T element)
    {
        _LOG_DEBUG_S("[MoveSemanticsTest] Addding.....")
        std::this_thread::sleep_for(sleepTime);
        add(element);
        return *this;
    }

    template<typename T>
    inline std::vector<T>& MoveSemanticsTest<T>::getElements()
    {
        return myVector;
    }

    template<typename T>
    inline void MoveSemanticsTest<T>::forEach(std::function<void(T&)>&& action)
    {
        for(auto& val : myVector)
        {
            action(val);
        }
    }

    template<typename T>
    inline int MoveSemanticsTest<T>::getNumber()
    {
        std::this_thread::sleep_for(sleepTime);
        return std::rand();
    }

}


#endif // MOVESEMANTICSTEST_H
