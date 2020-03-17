#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace daemonize {

    template<typename T>
    class SyncQueue
    {
    public:

        SyncQueue();
        ~SyncQueue();
        SyncQueue(const SyncQueue& other) = delete;
        SyncQueue(SyncQueue&& other) = delete;

        SyncQueue& operator=(const SyncQueue& other) = delete;
        SyncQueue& operator=(SyncQueue&& other) = delete;

        bool stopped();
        void enqueue(T element);
        T dequeue();
        void cleanUp();

    private:

        std::mutex myMutex;
        std::condition_variable myCondVar;
        std::queue<T> myQueue;
        std::atomic<bool> stopCondition;
    };

    template<typename T>
    inline SyncQueue<T>::SyncQueue()
        : myMutex()
        , myCondVar()
        , myQueue()
        , stopCondition(false)
    {}

    template<typename T>
    inline SyncQueue<T>::~SyncQueue()
    {}

    template<typename T>
    inline bool SyncQueue<T>::stopped()
    {
        return stopCondition;
    }

    template<typename T>
    inline void SyncQueue<T>::enqueue(T element)
    {
        std::unique_lock<std::mutex> lock(myMutex);
        myQueue.push(element);
        if(myQueue.size() == 1)
        {
            myCondVar.notify_one();
        }
    }

    template<typename T>
    inline T SyncQueue<T>::dequeue()
    {
        T element = nullptr;
        std::unique_lock<std::mutex> lock(myMutex);

        while(((myQueue.empty() == true) && (stopCondition == false)))
        {
            myCondVar.wait(lock);
        }

        if (stopCondition == false)
        {
            element = myQueue.front();
            myQueue.pop();
        }

        return element;
    }

    template<typename T>
    inline void SyncQueue<T>::cleanUp()
    {
        std::unique_lock<std::mutex> lock(myMutex);
        stopCondition = true;

        while(myQueue.empty() == false)
        {
            auto func = myQueue.front();
            myQueue.pop();
            delete func;
        }

        //notify service thread to stop
        myCondVar.notify_one();
    }
}


#endif // SYNCQUEUE_H
