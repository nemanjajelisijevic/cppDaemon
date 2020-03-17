#include "cppldratest.h"
#include "daemon.h"
#include "movesemanticstest.h"
#include "queuedconsumer.h"
#include <iostream>


namespace daemonize {

    cppldratest::cppldratest()
        : cnt(0)
        , testBool(false)
        , testBoolTwo{}
        , testBoolThree{true}
        , testBoolFour{}
        , testBoolFive{}
        , testBoolSix{true}
    {}


    void cppldratest::testCode()
    {

        Consumer* myConsumer = new QueuedConsumer;
        myConsumer->setName("Common Verifier Consumer");

        ManagedAsyncObject<MoveSemanticsTest<int>> testObject(myConsumer, new MoveSemanticsTest<int>{});

        testObject.setName("MoveSemanticsTest<int> Async Object");

        MoveSemanticsTest<int>& testPrototype = testObject.getPrototype();

        testPrototype.addd(0)
                .addd(1)
                .addd(0)
                .addd(1)
                .addd(0)
                .add(1);

        testPrototype.forEach(
            [](const int& val)
        {
            std::cout << "Prototype forEach: " << val << std::endl;
        });

        testObject.getPrototype().setSleepTime(std::chrono::seconds(1));

        testObject.async(&MoveSemanticsTest<int>::add, 0).noClosure();

        testObject.async(&MoveSemanticsTest<int>::add, 1)
        .setClosure(
        [testObject]()
        {
            testObject.getPrototype().forEach(
                        [](const int& value)
            {
                std::cout << "Prototype val " << value << std::endl;
            });
        });

        testObject.async(&MoveSemanticsTest<int>::addd, 0).setRecursiveClosure(
            [this, testObject](MoveSemanticsTest<int>& retPrototype, auto thisWrapper)
        {
            testObject.async(&MoveSemanticsTest<int>::forEach, [](const int& val) { std::cout << val << std::endl;}).setClosure(
                [this, testObject, thisWrapper] () mutable
            {
                if (testBool)
                {
                    testObject.async(&MoveSemanticsTest<int>::add, 1).setClosure(
                        [this, testObject]()
                    {
                            testBoolSix = false;

                            std::cout << "Prototype vector size: " << testObject.getPrototype().getElements().size();

                            for(const auto& el : testObject.getPrototype().getElements())
                            {

                                std::cout << "Vector element: " << el << std::endl;
                            }
                    });
                }
            });
        });
    }

    void cppldratest::testIndentation()
    {
        if (testBool)
        {
            if(testBoolTwo)
            {
                if(testBoolThree)
                {
                    if(testBoolFour)
                    {
                        if (testBoolFive)
                        {
                            if (testBoolSix)
                            {

                            }
                            else
                            {
                                if(testBoolFive)
                                {
                                    testCode();
                                }
                            }
                        }
                        else
                        {

                        }
                    }
                    else
                    {
                        testCode();
                    }
                }
                else
                {
                    testCode();
                }
            }
            else
            {
                testCode();
            }
        }
        else
        {
            testCode();
        }
    }
}

