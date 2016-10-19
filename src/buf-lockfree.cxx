#include <boost/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/atomic.hpp>

using namespace std;

#define UDP_LEN 500
#define CAPACITY 1024

boost::atomic_int producer_count (0);
boost::atomic_int consumer_count (0);

//boost::lockfree::spsc_queue<int, boost::lockfree::capacity<CAPACITY> > spsc_queue;
boost::lockfree::spsc_queue<int, boost::lockfree::fixed_sized<false> > spsc_queue(CAPACITY);

boost::array<uint32_t, UDP_LEN> data_buffer;
boost::asio::ip::udp::endpoint endpoint;

boost::shared_ptr<boost::asio::io_service> service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
boost::shared_ptr<boost::asio::io_service::work> work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*service));
boost::thread_group group;
boost::shared_ptr<boost::asio::ip::udp::socket> udpsocket = boost::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 2224)));

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket);
void handle_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket);

void WorkerThread7(boost::shared_ptr<boost::asio::io_service> service)
{
    //cout << "[" << boost::this_thread::get_id() << "] thread started" << endl;
    service->run();
    cout << "[" << boost::this_thread::get_id() << "] thread closed" << endl;
}

boost::atomic<bool> done (false);

void shut_down_socket()
{
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
    ++producer_count;
    //cout << "[" << boost::this_thread::get_id() << "] consumer : " << data_buffer.at(0) << endl;
    if(!spsc_queue.push(data_buffer.at(0))) {
        cout << "unable to push: " << data_buffer.at(0) << endl;
    }
    if(done) {
        shut_down_socket();
        return;
    }
    else {
        handle_data(socket);
    }
}

void read_data()
{
    int value;
    while(!service->stopped()) {
        while(spsc_queue.pop(value)) {
            cout << "[" << boost::this_thread::get_id() << "] " << value << endl;
            ++consumer_count;
        }
    }

}

int main()
{
    cout << "lockfree test" << endl;

    cout << "main thread : " << boost::this_thread::get_id() << endl;
    if(udpsocket->is_open()) {
        cout << "socket open" << endl;
    }

    cout << "boost::lockfree::queue is ";
    if(!spsc_queue.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    for(int i = 0; i < 2; i++) {
        group.create_thread(boost::bind(&WorkerThread7, service)); 
    }
    service->post(boost::bind(&handle_data, udpsocket));
    service->post(boost::bind(&read_data));

    while(!done) {
        if(consumer_count>=15000) {
            cout << "DONE" << endl;
            service->stop();
            group.join_all();
            done = true;
        }
    }


    cout << "produced : " << producer_count << endl;
    cout << "consumed : " << consumer_count << endl;

    return 0;
}
