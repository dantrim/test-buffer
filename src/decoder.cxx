#include "decoder.hh"
#include <iostream>
#include <boost/asio.hpp>


boost::mutex glock;

using std::cout;
using std::endl;




Decoder::Decoder() :
    m_buffer(NULL),
    n_received(0)
{
}

void Decoder::initialize(boost::shared_ptr<bounded_buffer<int> > buf)
{
    m_buffer = buf;
    glock.lock();
    cout << "decoder initialize m_buffer address: " << m_buffer << endl;
    glock.unlock();
}

void Decoder::read_data()
{
 //   while(m_buffer->get_size()) {
 //       cout << "Decoder::read_data() " << boost::this_thread::get_id() << endl;

 //   }
    
    //while(m_buffer->get_size()){
    int current_data;
    m_buffer->pop_back(&current_data);
    glock.lock();
    cout << "Decoder::read_data [" << boost::this_thread::get_id() << "]    (# received: " << n_received << ")  data: " << current_data << "  bounded buffer [size/capacity] : [" << m_buffer->get_size() << "/" << m_buffer->get_capacity() << "]" << endl; //"  " << m_buffer->print() << endl;
    if(n_received!=current_data)  cout << "NOPE" << endl;
    cout << endl;
    //cout << "decoder::read_data   " << current_data << endl;
    glock.unlock();
    //}
    n_received++;

    read_data();
    

}
