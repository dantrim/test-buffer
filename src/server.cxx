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
    m_bounded_buffer(NULL),
    m_queue(NULL),
    gathering_in_progress(false)
{
    m_circular_buffer.set_capacity(10);
}

void WorkerThread(boost::shared_ptr<boost::asio::io_service> io_service)
{
    //stream_lock.lock();
    cout << "WorkerThread    listening start [" << boost::this_thread::get_id() << "]" << endl;
    //stream_lock.unlock();

    io_service->run();

    //stream_lock.lock();
    cout << "WorkerThread    listening finish [" << boost::this_thread::get_id() << "]" << endl;
    //stream_lock.unlock();
}


void Server::initialize(boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<bounded_buffer<int> > buffer)
{
    m_bounded_buffer = buffer;

    int port = 2224;

    if(m_socket) {
        if(m_socket->is_open()) {
            m_socket->close();
            boost::system::error_code ec;
            m_socket->shutdown(ip::udp::socket::shutdown_both, ec);
        }
    }

    m_io_service = io;
    //m_io_service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    //m_idle_work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*m_io_service));
    //m_strand = boost::shared_ptr<boost::asio::io_service::strand>(new boost::asio::io_service::strand(*m_io_service));
    m_socket = boost::shared_ptr<boost::asio::ip::udp::socket>(new ip::udp::socket(*m_io_service, ip::udp::endpoint(ip::udp::v4(), port)));

    //m_bounded_buffer = boost::shared_ptr<bounded_buffer<int>>(new bounded_buffer<int>(1000000));
    //m_queue = boost::shared_ptr<queue<int>>(new queue<int>(100));

//    stream_lock.lock();
//    cout << "server buffer address: " << m_bounded_buffer << endl;
//    cout << "server queue address : " << m_queue << endl;
//    stream_lock.unlock();

    m_decoder = boost::shared_ptr<Decoder>(new Decoder());
    m_decoder->initialize(m_io_service, m_bounded_buffer);
    //m_decoder->initialize(m_queue);

    //m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, m_io_service));
    //m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, m_io_service));

    if(!m_socket->is_open()) {
        cout << "ERROR opening socket on port: " << port << endl;
        exit(1);
    }


    //start_gathering();
}

void Server::listen()
{
    stream_lock.lock();
    cout << "Server::listen() from main thread: " << boost::this_thread::get_id() << endl;
    stream_lock.unlock();


    for(int i = 0; i < m_thread_count; i++) {
        m_thread_group.create_thread( boost::bind( &WorkerThread, m_io_service) );
    }
    //boost::thread decode_thread(boost::bind(&Decoder::read_data, m_decoder));
    m_io_service->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_io_service->post(boost::bind(&Decoder::read_data, m_decoder));
    m_io_service->run();
    //m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, m_io_service));
    //m_io_service->run();


  //  boost::thread decoding_thread(boost::bind(&Decoder::read_data, m_decoder));
    //boost::thread decoding_thread(boost::bind(&Decoder::read_queue, m_decoder));

   // boost::thread receiver_thread1(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
   // boost::thread receiver_thread2(boost::bind(&Server::handle_data, this, boost::ref(n_count)));

    
 //   stream_lock.lock();
 //   cout << "strand: " << m_strand << endl;
 //   stream_lock.unlock();

    //boost::thread receiver_thread(boost::bind(&Server::handle_data, this, boost::ref(n_count)));


    //m_thread_group.create_thread(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_thread_group.create_thread(boost::bind(&Decoder::read_data, m_decoder));

    //m_strand->post(boost::bind(&Decoder::read_data, m_decoder));
    //m_strand->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //boost::thread decode_thread(boost::bind(&Decoder::read_data, m_decoder));

 //   m_io_service->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //stream_lock.lock();
    //cout << "Server thread: " << boost::this_thread::get_id() << endl;
    //stream_lock.unlock();
    //m_strand->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_io_service->post(boost::bind(&Decoder::read_data, m_decoder));
    //m_strand->post(boost::bind(&Server::handle_data, this, boost::ref(n_count)));
    //m_strand->post(boost::bind(&Decoder::read_data, &m_decoder));

}

void Server::start_gathering()
{
    if(gathering_in_progress) return;
    else {
        for(int i = 0; i < m_thread_count; i++) {
            m_thread_group.create_thread( boost::bind( &WorkerThread, m_io_service) );
        }
        m_io_service->post(boost::bind(&Decoder::read_data, m_decoder));
        gathering_in_progress = true;
    }

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
    //stream_lock.lock();
    //cout << "Server::decode_data    [" << boost::this_thread::get_id() << "]  receiving from: " << ip_ << endl;
  ////  int n_x = 0;
  ////  for(const auto& x : m_data_buffer) {
  ////      if(x==4321) break;
  ////      n_x++;
  ////  }
  ////  cout << "incoming size: " << n_x << endl;
    //stream_lock.unlock();
    //int blah = (int)m_data_buffer.at(0);
    //std::cout << m_data_buffer.at(0) << std::endl;
    add_to_buffer(m_data_buffer.at(0));
    //add_to_buffer(blah);
    //add_to_buffer((int)m_data_buffer.at(0));
    //m_bounded_buffer->push_front(counter);

    counter++;
    if(counter>10000)
    {
        end();
    }
    //else {
   // { cout << "n decoder received: " << m_decoder->nReceived() << "  on thread: " << m_decoder->thread() << endl; } //end(); }// m_decoder->print_final(); } // end(); } //m_decoder->print_final(); } //end(); }
    else {
    handle_data(counter);
    }
    
}

void Server::add_to_buffer(int counter)
{
    //cout << "Server::add_to_buffer()" << endl;
    m_bounded_buffer->push_front(counter);
    //m_queue->push(counter);
}

void Server::end()
{
    m_io_service->stop();
    //for(int i = 0; i< 100; i++) {}
    //cout << "final count read: " << n_count << endl;
    //m_decoder->print_final();
}
