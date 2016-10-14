#ifndef SERVER_HH
#define SERVER_HH

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <boost/circular_buffer.hpp>
//#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/call_traits.hpp>
#include <boost/bind.hpp>

//#include "bounded_buffer.hh"
#include "decoder.hh"

using boost::asio::ip::udp;
namespace ip = boost::asio::ip;

#define MAXBUFLEN 65507

class Server : public boost::enable_shared_from_this<Server>
{
    public :
        Server();
        virtual ~Server(){};

        void initialize(boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<bounded_buffer<int>> buf);

        void listen();
        void start_gathering();

        void handle_data(int& count);

        void decode_data(int& count, size_t& bytes, const boost::system::error_code error, std::size_t size_);

        void add_to_buffer(int counter );

        void end();


    private :
        boost::array<uint32_t, MAXBUFLEN> m_data_buffer;

        int n_count;
        int m_thread_count;

        boost::shared_ptr<boost::asio::ip::udp::socket> m_socket;
        boost::thread_group m_thread_group;
        boost::thread m_decoding_thread;
        boost::shared_ptr<boost::asio::io_service> m_io_service;
        boost::shared_ptr<boost::asio::io_service::work> m_idle_work;
        boost::shared_ptr<boost::asio::io_service::strand> m_strand;
        boost::asio::ip::udp::endpoint m_remote_endpoint;

        boost::circular_buffer<uint32_t> m_circular_buffer;
        boost::shared_ptr<bounded_buffer<int> > m_bounded_buffer;
        boost::shared_ptr<queue<int> > m_queue;

        boost::shared_ptr<Decoder> m_decoder;
        bool gathering_in_progress;

}; // class


#endif
