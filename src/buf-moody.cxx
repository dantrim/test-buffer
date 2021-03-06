#include "readerwriterqueue.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "atomicops.h"
#include <thread>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>


int n_count = 0;

#define UDP_LEN 600
#define MC_CAPACITY 1023
boost::array<uint32_t, UDP_LEN> data_buffer;
boost::asio::ip::udp::endpoint endpoint;
boost::shared_ptr<int> n_packet = boost::shared_ptr<int>(new int(0));

//(*n_packet)=0;

    boost::shared_ptr<boost::asio::io_service> service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    boost::shared_ptr<boost::asio::io_service::work> work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*service));
    boost::thread_group group;
    boost::shared_ptr<boost::asio::ip::udp::socket> udpsocket = boost::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 2224)));

#include <iostream>
using namespace std;
#include <sstream>

using namespace moodycamel;
boost::shared_ptr<BlockingReaderWriterQueue<boost::array<uint32_t, UDP_LEN> > > q = boost::shared_ptr<BlockingReaderWriterQueue< boost::array<uint32_t, UDP_LEN > > >(new BlockingReaderWriterQueue< boost::array<uint32_t, UDP_LEN> >(MC_CAPACITY));
//boost::shared_ptr<BlockingReaderWriterQueue<int> > q = boost::shared_ptr<BlockingReaderWriterQueue<int> >(new BlockingReaderWriterQueue<int>());

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, int& count);
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
    //int item = -1;
    boost::array<uint32_t, UDP_LEN> incoming_data;
    while(!service->stopped()){// && !(*n_packet>=35000)) {
        //while(q->peek()) {
        //if(q->peek()) {
        //if(q->try_dequeue(item))
            if(q->peek()) {
                //q->wait_dequeue(item);
                //boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                if(q->try_dequeue(incoming_data)) {
                //if(q->wait_dequeue_timed(incoming_data, std::chrono::milliseconds(1))) {
                int first = incoming_data.at(0);
                for(auto& x : incoming_data) {
                    if(x==4321) {
                        (*n_packet)++;
                        //n_count++;
                        cout << "d" << endl;
                        //cout << "[" << boost::this_thread::get_id() << "] read : " << first << endl;
                    }
                }
                //cout << "[" << boost::this_thread::get_id() << "] read : " << 
                //if(q->wait_dequeue_timed(item, std::chrono::milliseconds(5))) {
                    //q.wait_dequeue(item);
                //if(item%10000==0)
                //cout << "[" << boost::this_thread::get_id() << "] read_data : " << item << endl;
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
        boost::bind(receive_data, socket, boost::ref(*n_packet)));
}
void look_its_data(boost::array<uint32_t, 65507>& data_buffer)
{
    auto first = data_buffer.at(0);
    for(auto& x : data_buffer) {
        if(x==4321) {
            cout << "[" << boost::this_thread::get_id() << "] look : " << first << endl;
        }
    }
    //boost::this_thread::sleep(boost::posix_time::milliseconds(1));
}

void receive_data(boost::shared_ptr<boost::asio::ip::udp::socket> socket, int& count)
{
    //look_its_data(data_buffer);
    //handle_data(socket);
    q->enqueue(data_buffer);
    //(count)++;
    //int check = (*n_packet);
    //if(check%10000==0 && check<=35000) 
    //cout << "[" << boost::this_thread::get_id() << "] receive_data : " << data_buffer.at(0) << endl;
    //if((*n_packet)>=35000) {
    if((count)>=35000) {
        shut_down_socket();
        return;
      
      //cout << "[" << boost::this_thread::get_id() << "]  shutting down" << endl;
      //service->stop();
      //shut_down_socket();
      //service->reset();
    }
    else {
    handle_data(socket);
    }
}
int main()
{

/*
    typedef std::pair<std::string, int> test_array;

    int test_x = 48;
    std::string ip_test = "4.3.2.1";
    test_array yep = std::make_pair(ip_test, test_x);
    cout << "test yep ip: " << yep.first << ", second: " << yep.second << endl;
    return 0;

    BlockingReaderWriterQueue<std::map<int, int> > mapping_queue(100);
    BlockingReaderWriterQueue<std::pair<int, int> > pair_queue(100);
    BlockingReaderWriterQueue<std::pair<std::string, boost::array<uint32_t, 65507> > > boost_queue(20);

    stringstream ipstream;
    for(int i = 0; i < 11; i++) {
        ipstream << i << "." << i << "." << i << "." << i;
        boost::array<uint32_t, 65507> testbuf;
        for(int j = 0; j < 5; j++) {
            testbuf.at(j) = j*i;
        }
        testbuf.at(5) = 4321;
        boost_queue.enqueue(std::make_pair(ipstream.str(), testbuf));
        ipstream.str(""); 
        //pair_queue.enqueue(std::make_pair(i, 2*i));
    }
   // //pair_queue.enqueue(std::make_pair(1,400)); 
   // for(int i = 0; i < 50; i++) {
   //     std::pair<int, int> read_in;
   //     bool success = pair_queue.try_dequeue(read_in);
   //     if(success) {
   //         cout << "read data: <IP, DATA> : <" << read_in.first << ", " << read_in.second << ">" << endl;
   //     }
   //     else {
   //         cout << "could not read in data " << i << endl;
   //     }

   // }
    // boost_queue
    for(int i =0 ; i < 10; i++) {
        std::pair<std::string, boost::array<uint32_t, 65507> > read_in;
        bool success = boost_queue.try_dequeue(read_in);
        if(success) {
            std::string ip_ = read_in.first;
            stringstream input;
            for(auto x : read_in.second) { 
                if(x==4321) break;
                input << x << "  ";
            }
            cout << "IP: " << ip_ << "  >> " << input.str() << endl;
        }
        else {
            cout << "could not read in data " << i << endl;
        }
    }
*/
    cout << "main thread : " << boost::this_thread::get_id() << endl;
    if(udpsocket->is_open()) {
        cout << "socket open" << endl;
    }
    cout << "q address main " << q << endl;


    for(int i = 0; i < 2; i++) {
        group.create_thread(boost::bind(&WorkerThread5, service)); 
    }
    service->post(boost::bind(&handle_data, udpsocket));
    service->post(boost::bind(&read_data));

    //boost::thread listener(boost::bind(&handle_data, udpsocket));
    //boost::thread reader(boost::bind(&read_data));
    //service->run();

    //boost::thread listener(boost::bind(&read_data));
    //service->run();

    cout << "hello world" << endl;
    while(!service->stopped()) {
        if((*n_packet>=35000)) {
            //reader.join();
            //listener.join();
            //shut_down_socket();
            service->stop();
            group.join_all();
            //shut_down_socket();
            cout << "shutting down socket at " << (*n_packet) << " events" << endl;
            cout << "n_count = " << n_count << endl;
            //cout << (*n_packet) << endl;
        }
    }

    //boost::thread listener(boost::bind(&handle_data, udpsocket));
    //boost::thread reader(boost::bind(&read_data));
    //service->run();

    //listener.join();
    //reader.join();

    //listener.join();
    //group.join_all();
    //cout << "join_all" << endl;
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
