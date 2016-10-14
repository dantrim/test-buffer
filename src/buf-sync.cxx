
#include <iostream>
using namespace std;

#include "SynchronisedQueue.hh"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

boost::array<uint32_t, 65507> data_buffer;
boost::asio::ip::udp::endpoint endpoint;
int packet_count = 0;


    boost::shared_ptr<boost::asio::io_service> service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    boost::shared_ptr<boost::asio::io_service::work> work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*service));
    boost::shared_ptr<boost::asio::io_service::strand> strand = boost::shared_ptr<boost::asio::io_service::strand>(new boost::asio::io_service::strand(*service));
    boost::thread_group group;

boost::shared_ptr<boost::asio::ip::udp::socket> udpsocket = boost::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 2224)));
void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, SynchronisedQueue<int>* queue);
void handle_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, SynchronisedQueue<int>* queue);

void WorkerThread4(boost::shared_ptr<boost::asio::io_service> ioservice)
{
    ioservice->run();
}

void client_writer (int myid, SynchronisedQueue<int> *m_queue) {
    int i = 0;
    cout << "[" << boost::this_thread::get_id() << "] client_writer writing" << endl;
    while(++i < 5) m_queue->Enqueue(i*myid);
}

void client_reader (int myid, SynchronisedQueue<int> *m_queue, boost::shared_ptr<boost::asio::io_service> srv){
    bool start = false;
    //while(start && (m_queue->Size() > 0)) {
    //while(m_queue->Size()>0) {
        start=true;
        //int x = m_queue->Dequeue();
        cout << "[" << boost::this_thread::get_id() << "] #" << myid << ":" << m_queue->Dequeue() << endl;
    //}
    client_reader(myid, m_queue, srv);
}

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, SynchronisedQueue<int>* queue)
{
  queue->Enqueue(data_buffer.at(0));
    packet_count++;
    handle_data(socket, queue);
}
void handle_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, SynchronisedQueue<int>* queue)
{
    udpsocket->async_receive_from(
        boost::asio::buffer(data_buffer), endpoint,
        boost::bind(receive_data, socket, queue) );
}


int main(){



    SynchronisedQueue<int> m_queue; // a global typedef

    for(int i = 0; i < 2; i++) {
        group.create_thread(boost::bind(&WorkerThread4, service));
    }
    cout << "main() called on thread: " << boost::this_thread::get_id() << endl;

    std::cin.get();

    //service->post(boost::bind(&client_writer, 1, &m_queue));
    service->post(boost::bind(&client_reader, 4, &m_queue, service));
    service->post(boost::bind(&handle_data, udpsocket, &m_queue));

    for(int i = 0; i < 10; i++) {
        std::cin.get();
        cout << "main [" << boost::this_thread::get_id() << "] sending data (" << i << ")" << endl;
        service->post(boost::bind(&client_writer, 1, &m_queue));
    }
    //service->post(boost::bind(&client_writer, 2, &m_queue));
    //service->post(boost::bind(&client_reader, 3, &m_queue));

    
    cout << "main [" << boost::this_thread::get_id() << "] service stop" << endl;
    service->stop();
    cout << "main [" << boost::this_thread::get_id() << "] join_all" << endl;
    group.join_all();

    //boost::thread worker1(boost::bind(&client_writer, 1, &m_queue));
    //boost::thread worker2(boost::bind(&client_writer, 2, &m_queue));
    //boost::thread worker3(boost::bind(&client_reader, 3, &m_queue));
    //worker1.join();
    //worker3.join();


    //boost::thread worker1(client_writer, boost::bind(1, &m_queue));
    //boost::thread worker2(client_writer, boost::bind(2, &m_queue));
    //boost::thread worker3(client_reader, boost::bind(3, &m_queue));
    //worker3.join(); //wait for 3 to finish before exiting

    return 0;
}
