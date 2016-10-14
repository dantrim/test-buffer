#include "decoder.hh"
#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

boost::mutex glock;

using std::cout;
using std::endl;




Decoder::Decoder() :
    m_buffer(NULL),
    m_queue(NULL),
    n_received(0),
    n_max_cap(-1),
    check_thread(true),
    m_event_counter(NULL)
{
}

void Decoder::initialize(boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<bounded_buffer<int> > buf)
{
    m_buffer = buf;
    m_io = io;
    glock.lock();
    cout << "decoder initialized on thread: " << boost::this_thread::get_id() << endl;
    glock.unlock();
}

//void Decoder::initialize(boost::shared_ptr<queue<int> > buf)
//{
//    m_queue = buf;
//    glock.lock();
//    cout << "decoder initialize m_queue address: " << m_queue << endl;
//    glock.unlock();
//}

void Decoder::start_gathering(boost::shared_ptr<int> event_count)
{
    m_event_counter = event_count;
    //m_io->post(boost::bind(&Decoder::read_data, this));
    boost::thread gather_thread(boost::bind(&Decoder::read_data, this));

}
void Decoder::stop_gathering()
{
    cout << "decoder received: " << nReceived() << " packets on thread: " << thread() << endl;
    //m_io->stop();
}

void Decoder::read_data()
{
    //glock.lock();
    //cout << "Decoder::read_data()" << endl;
    //glock.unlock();
 //   while(m_buffer->get_size()) {
 //       cout << "Decoder::read_data() " << boost::this_thread::get_id() << endl;

 //   }
    
    //while(m_buffer->get_size()){
    int current_data;
    m_buffer->pop_back(&current_data);
    yep_vec.push_back(current_data);
    if(check_thread) {
        std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
        m_thread_id = threadId;
        check_thread = false;
    }
    glock.lock();
    cout << "Decoder::read_data [" << *m_event_counter << boost::this_thread::get_id() << "]    (# received: " << n_received << ")  data: " << current_data << "  bounded buffer [size/capacity] : [" << m_buffer->get_size() << "/" << m_buffer->get_capacity() << "]"<< endl;// << "  " << m_buffer->print() << endl;
    ////if(m_buffer->get_size() > n_max_cap) { n_max_cap = m_buffer->get_size(); cout << "max cap is " << n_max_cap << endl; }
    ////if(n_received!=current_data)  cout << "NOPE" << endl;
    ////cout << endl;
    ////cout << "decoder::read_data   " << current_data << endl;
    glock.unlock();
    //}
    //(*m_event_counter)++;
    //if(*m_event_counter>10000) stop_gathering();
    n_received++;

    read_data();
}

void Decoder::read_queue()
{
    int current_data;
    m_queue->wait_and_pop(current_data);
    glock.lock();   
    cout << "Decoder::read_queue [" << boost::this_thread::get_id() << "]    (# received: " << n_received << ") data: " << current_data << " queue [size/cap] : [" << m_queue->get_size() << "/" << m_queue->get_capacity() << "] " << m_queue->print() << endl;
     cout << endl;
    glock.unlock();
    n_received++;
    read_queue();
     
}

void Decoder::print_final()
{
    for(int i = 0; i < yep_vec.size(); i++)
        cout << " > " << i << "  " << yep_vec.at(i) << endl;
}
