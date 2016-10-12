
#include <boost/thread/condition_variable.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/call_traits.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <string>
#include <iostream>
#include <sstream>


template <class T>
class queue
{
    public :
        typedef boost::circular_buffer<T> container_type;
        typedef typename container_type::size_type size_type;
        typedef typename container_type::value_type value_type;
        typedef typename boost::call_traits<value_type>::param_type param_type;

        explicit queue(size_type capacity) : m_container(capacity) {}

        void push(T const& data)
        {
            boost::mutex::scoped_lock lock(m_mutex);
            m_container.push_front(data);
            ++m_unread;
            lock.unlock();
            condition_variable.notify_one();
        }

        bool empty() const
        {
            boost::mutex::scoped_lock lock(m_mutex);
            return m_container.empty();
        }

        bool try_pop(T& popped_value)
        {
            boost::mutex::scoped_lock lock(m_mutex);
            if(m_container.empty()) {
                return false;
            }
            popped_value = m_container[--m_unread];
            m_container.pop_back();
            return true;
        }

        void wait_and_pop(T& popped_value)
        {
            boost::mutex::scoped_lock lock(m_mutex);
            while(m_container.empty()) {
                condition_variable.wait(lock);
            }
            popped_value = m_container[--m_unread];
            m_container.pop_back();
        }

        int get_capacity() {
            return m_container.capacity();
        }

        int get_size() {
            return m_container.size();
        }

        std::string print() {
            std::stringstream sx;
            for(int i = 0; i < (int)m_container.size(); i++) {
                sx << m_container[i] << " ";
            }
            return sx.str();
        }
        

    private :
        container_type m_container;
        size_type m_unread;
        mutable boost::mutex m_mutex;
        boost::condition_variable condition_variable;
        
}; // class
