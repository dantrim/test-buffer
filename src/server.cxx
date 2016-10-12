#include "server.hh"
#include <iostream>
#include "decoder.hh"
#include <string>
using std::cout;
using std::endl;

boost::mutex stream_lock;

Server::Server() :
    n_count(0),
    m_thread_count(1),
    m_socket(NULL),
    m_io_service(NULL),
    m_idle_work(NULL),
    m_strand(NULL),
    m_bounded_buffer(NULL)
{
    m_circular_buffer.set_capacity(10);
}

void WorkerThread(boost::shared_ptr<boost::asio::io_service> io_service)
{
    stream_lock.lock();
    cout << "WorkerThread    listening start [" << boost::this_thread::get_id() << "]" << endl;
    stream_lock.unlock();

    io_service->run();

    stream_lock.lock();
    cout << "WorkerThread    listening finish [" << boost::this_thread::get_id() << "]" << endl;
    stream_lock.unlock();
}


void Server::initialize(boost::shared_ptr<boost::asio::io_service> io)
{
    int port = 2224;

    if(m_socket) {
        if(m_socket->is_open()) {
            m_socket->close();
            boost::system::error_code ec;
            m_socket->shutdown(ip::udp::socket::shutdown_both, ec);
        }
    }

    m_io_service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    //m_io_service = io;
    m_idle_work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*m_io_service));
    m_strand = boost::shared_ptr<boost::asio::io_service::strand>(new boost::asio::io_service::strand(*m_io_service));
    m_socket = boost::shared_ptr<boost::asio::ip::udp::socket>(new ip::udp::socket(*m_io_service, ip::udp::endpoint(ip::udp::v4(), port)));

    m_bounded_buffer = boost::shared_ptr<bounded_buffer<int>>(new bounded_buffer<int>(100));

    stream_lock.lock();
    cout << "server buffer address: " << m_bounded_buffer << endl;
    stream_lock.unlock();

    m_decoder = boost::shared_ptr<Decoder>(new Decoder());
    m_decoder->initialize(m_bounded_buffer);

    //m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, m_io_service));
    //m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, m_io_service));

    if(!m_socket->is_open()) {
        cout << "ERROR opening socket on port: " << port << endl;
        exit(1);
    }

}

void Server::listen()
{
    stream_lock.lock();
    cout << "Server::listen()" << endl;
    stream_lock.unlock();

    

    //for(int i = 0; i < m_thread_count; i++) {
    //    m_thread_group.create_thread(boost::bind(WorkerThread, m_io_service));
    //}

    boost::thread decoding_thread(boost::bind(&Decoder::read_data, m_decoder));

    
 //   stream_lock.lock();
 //   cout << "strand: " << m_strand << endl;
 //   stream_lock.unlock();
    m_io_service->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_io_service->post(boost::bind(&Decoder::read_data, m_decoder));
    cout << "Server thread: " << boost::this_thread::get_id() << endl;
    //m_strand->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    m_io_service->run();
    //m_strand->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_strand->post(boost::bind(&Decoder::read_data, &m_decoder));

}

void Server::handle_data(int& counter)
{
    if(m_socket) {
        std::size_t nbytes = 0;
        m_socket->async_receive_from(
            boost::asio::buffer(m_data_buffer), m_remote_endpoint,
            boost::bind(&Server::decode_data, this,
            boost::ref(counter),
            boost::ref(nbytes),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
        );
    }
    else {
        cout << "socket is null" << endl;
        exit(1);
    }

}

void Server::decode_data(int& counter, size_t& bytes, const boost::system::error_code error, std::size_t size_)
{
    bytes = size_;
    std::string ip_ = m_remote_endpoint.address().to_string();
    stream_lock.lock();
    //cout << "Server::decode_data    receiving from: " << ip_ << "  counter: " << counter << "  bounded buffer [size/capacity] : [" << m_bounded_buffer->get_size() << "/" << m_bounded_buffer->get_capacity() << "]" << endl;
    //cout << "Server::decode_data    receiving from: " << ip_ << endl;
    stream_lock.unlock();
    //int blah = (int)m_data_buffer.at(0);
    //std::cout << m_data_buffer.at(0) << std::endl;
    add_to_buffer(m_data_buffer.at(0));
    //add_to_buffer(blah);
    //add_to_buffer((int)m_data_buffer.at(0));
    //m_bounded_buffer->push_front(counter);

    counter++;

    handle_data(counter);
    
}

void Server::add_to_buffer(int counter)
{
    //cout << "Server::add_to_buffer()" << endl;
    m_bounded_buffer->push_front(counter);
}
