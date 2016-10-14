#include <iostream>
#include "server.hh"
#include "decoder.hh"

using std::cout;
using std::endl;

int main()
{
    cout << "buf-server" << endl;

    boost::shared_ptr<Server> server = boost::shared_ptr<Server>(new Server());
    boost::shared_ptr<Decoder> decoder = boost::shared_ptr<Decoder>(new Decoder());
    boost::shared_ptr<boost::asio::io_service> io_service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
    boost::shared_ptr<boost::asio::io_service::work> work = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(*io_service));

    //boost::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*io));
    //boost::thread_group threadPool;
    //for(int i = 0; i < 2; i++) {
    //    threadPool.create_thread(boost::bind(&boost::asio::io_service::run, io_service));
    //}

    boost::shared_ptr<bounded_buffer<int> > buf = boost::shared_ptr<bounded_buffer<int> >(new bounded_buffer<int>(1000));

    boost::shared_ptr<int> event_count = boost::shared_ptr<int>(new int());
    *event_count = 0;

    server->initialize(io_service, buf);//, event_count);
    //decoder->initialize(io_service, buf);
    
    server->listen();
    //decoder->start_gathering(event_count);
    //io_service->run();

    //while(*event_count<20) decoder->stop_gathering();
    //cout << "the end" << endl;

    //while(!io_service->stopped()) {}
    //decoder->stop_gathering();
    //server->end();

    return 0;
}
