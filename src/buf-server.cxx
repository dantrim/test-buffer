#include <iostream>
#include "server.hh"

using std::cout;
using std::endl;

int main()
{
    cout << "buf-server" << endl;

    Server* server = new Server();
    boost::shared_ptr<boost::asio::io_service> io(new boost::asio::io_service());
    //boost::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*io));
    //boost::thread_group threadPool;
    //for(int i = 0; i < 2; i++) {
    //    threadPool.create_thread(boost::bind(&boost::asio::io_service::run, io));
    //}
       
    server->initialize(io);
    server->listen();
    //server->end();

    return 0;
}
