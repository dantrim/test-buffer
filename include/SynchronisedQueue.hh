#ifndef _QUEUE_
#define _QUEUE_

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <queue>

using namespace std;

template <typename T>
class SynchronisedQueue {

    private :
        std::queue<T> m_queue; // Use STL queue to store data
        boost::mutex m_mutex;
        boost::condition_variable m_cond;
        long m_size;

    public :
        SynchronisedQueue() : m_size(0) {}
        virtual ~SynchronisedQueue() throw () {}

        long Size() { return m_size; }

        // Add data to the queue and notify others
        void Enqueue(const T& data)
        {
            // acquire lock on the queue
            //boost::unique_lock<boost::mutex> lock(m_mutex);
            // add data
            //cout << "[" << boost::this_thread::get_id() << "] Enqueue adding " << data << " size: " << m_size << ", " << m_queue.size() << endl;
            m_queue.push(data);
            m_size++;
            // notify others that data is ready
            //lock.unlock();
            m_cond.notify_one();
        } // lock automatically released

        // get data from the queue, wait for data if not available
        T Dequeue()
        {
            // acquire lock on the queue
            boost::unique_lock<boost::mutex> lock(m_mutex);
            // when there is no data, wait till someone fills it.
            // lock is automatically released in the wait and obtained
            // again after the wait
            while (m_queue.size()==0) { /*cout << "reader waiting..." << endl;*/ m_cond.wait(lock); }
            // retrieve the data from the queue
            T result = m_queue.front(); m_queue.pop(); m_size--;
            return result;
        }

}; // class



#endif
