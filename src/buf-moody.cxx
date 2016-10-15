#include "readerwriterqueue.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "atomicops.h"
#include <thread>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>


boost::array<uint32_t, 65507> data_buffer;
boost::asio::ip::udp::endpoint endpoint;
boost::shared_ptr<int> n_packet = boost::shared_ptr<int>(new int(0));

//(*n_packet)=0;

    boost::shared_ptr<boost::asio::io_service> service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    boost::shared_ptr<boost::asio::io_service::work> work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*service));
    boost::thread_group group;
    boost::shared_ptr<boost::asio::ip::udp::socket> udpsocket = boost::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 2224)));

#include <iostream>
using namespace std;

using namespace moodycamel;
boost::shared_ptr<BlockingReaderWriterQueue<int> > q = boost::shared_ptr<BlockingReaderWriterQueue<int> >(new BlockingReaderWriterQueue<int>());

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket);
void handle_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket);

//BlockingReaderWriterQueue<int> q;


void WorkerThread5(boost::shared_ptr<boost::asio::io_service> service)
{
    cout << "[" << boost::this_thread::get_id() << "] thread started" << endl;
    service->run();
    cout << "[" << boost::this_thread::get_id() << "] thread closed" << endl;
}

void read_data()
{
    int item = -1;
    while(!service->stopped()){// && !(*n_packet>=63040)) {
        //while(q->peek()) {
        //if(q->peek()) {
        //if(q->try_dequeue(item))
            if(q->peek()) {
                //q->wait_dequeue(item);
                //boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                if(q->wait_dequeue_timed(item, std::chrono::milliseconds(1))) {
                //if(q->wait_dequeue_timed(item, std::chrono::milliseconds(5))) {
                    //q.wait_dequeue(item);
                if(item%10000==0)
                cout << "[" << boost::this_thread::get_id() << "] read_data : " << item << endl;
                }
            }
            //else if(item==30000) { break; }
    }
    //if(!service->stopped())
    //    read_data();
}
void shut_down_socket() {
    if(udpsocket->is_open()) {
        udpsocket->close();
        boost::system::error_code ec;
        udpsocket->shutdown(boost::asio::ip::udp::socket::shutdown_both, ec);
    }
}

void handle_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket)
{
    socket->async_receive_from(
        boost::asio::buffer(data_buffer), endpoint,
        boost::bind(receive_data, socket));
}

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket)
{
    q->enqueue(data_buffer.at(0));
    //if((*n_packet)%10000==0)
    //cout << "[" << boost::this_thread::get_id() << "] receive_data : " << data_buffer.at(0) << endl;
    (*n_packet)++;
    if((*n_packet)>=63040) {
        
        cout << "[" << boost::this_thread::get_id() << "]  shutting down" << endl;
        service->stop();
        shut_down_socket();
        service->reset();
    }
    else {
    handle_data(socket);
    }
}
int main()
{
    cout << "main thread : " << boost::this_thread::get_id() << endl;
    if(udpsocket->is_open()) {
        cout << "socket open" << endl;
    }
    cout << "q address main " << q << endl;


    for(int i = 0; i < 2; i++) {
        group.create_thread(boost::bind(&WorkerThread5, service)); 
    }
    //boost::thread worker1(boost::bind(&boost::asio::io_service::run, service));
    //boost::thread listener(boost::bind(&handle_data, udpsocket));
    service->post(boost::bind(&handle_data, udpsocket));
    service->post(boost::bind(&read_data));
    //boost::thread listener(boost::bind(&read_data));
    //service->run();

    cout << "hello world" << endl;
    while(!service->stopped()) {
        if((*n_packet>=63040)) {
            service->stop();
            //shut_down_socket();
            cout << "shutting down socket at " << (*n_packet) << " events" << endl;
            //cout << (*n_packet) << endl;
        }
    }

    //boost::thread listener(boost::bind(&handle_data, udpsocket));
    //boost::thread reader(boost::bind(&read_data));
    //service->run();

    //listener.join();
    //reader.join();

    //listener.join();
    group.join_all();
    cout << "join_all" << endl;
/*
    std::thread reader([&]() {
        int item;
        //for (int i = 0; i != 10000; ++i) {
        while(true) {
            // Fully-blocking:
            q->wait_dequeue(item);
            cout << "dq'd item: " << item << endl;
    
            //// Blocking with timeout
            //if (q.wait_dequeue_timed(item, std::chrono::milliseconds(5)))
            //    ++i;
        }
    });
    std::thread writer([&]() {
        for (int i = 0; i != 10000; ++i) {
            //cout << "enqueing " << i << endl;
            q->enqueue(i);
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    });
*/
/*
    int item;
    for(int i = 0 ; i < 10000; ++i) {
        q->enqueue(i);
        cout << "enq : " << i << endl;
        if(rand()%50==0 && q->peek()!=nullptr) {
            q->wait_dequeue_timed(item, std::chrono::milliseconds(1));
            cout << "dq'd item: " << item << endl;
        }
    }

    std::cin.get();
    cout << "getting rest" << endl;

    //while(true) {
    // only continue if non-empty
    while(q->peek()) {
        q->wait_dequeue_timed(item, std::chrono::milliseconds(1));
        cout << "dq'd item: " << item << endl;
    }

    //writer.join();
    //boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

    //reader.join();
    
    assert(q->size_approx() == 0);
*/
    return 0;
}
