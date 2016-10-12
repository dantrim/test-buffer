#include "decoder.hh"
#include <iostream>
#include <boost/asio.hpp>


boost::mutex glock;

using std::cout;
using std::endl;




Decoder::Decoder() :
    m_buffer(NULL),
    m_queue(NULL),
    n_received(0),
    n_max_cap(-1)
{
}

void Decoder::initialize(boost::shared_ptr<bounded_buffer<int> > buf)
{
    m_buffer = buf;
    glock.lock();
    cout << "decoder initialize m_buffer address: " << m_buffer << endl;
    glock.unlock();
}

//void Decoder::initialize(boost::shared_ptr<queue<int> > buf)
//{
//    m_queue = buf;
//    glock.lock();
//    cout << "decoder initialize m_queue address: " << m_queue << endl;
//    glock.unlock();
//}

void Decoder::read_data()
{
 //   while(m_buffer->get_size()) {
 //       cout << "Decoder::read_data() " << boost::this_thread::get_id() << endl;

 //   }
    
    //while(m_buffer->get_size()){
    int current_data;
    m_buffer->pop_back(&current_data);
    //yep_vec.push_back(current_data);
    //glock.lock();
    //cout << "Decoder::read_data [" << boost::this_thread::get_id() << "]    (# received: " << n_received << ")  data: " << current_data << "  bounded buffer [size/capacity] : [" << m_buffer->get_size() << "/" << m_buffer->get_capacity() << "]"<< endl;// << "  " << m_buffer->print() << endl;
    //if(m_buffer->get_size() > n_max_cap) { n_max_cap = m_buffer->get_size(); cout << "max cap is " << n_max_cap << endl; }
    //if(n_received!=current_data)  cout << "NOPE" << endl;
    //cout << endl;
    //cout << "decoder::read_data   " << current_data << endl;
    //glock.unlock();
    //}
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
