#ifndef DECODER_HH
#define DECODER_HH

#include "bounded_buffer.hh"
#include "queue.hh"
#include <vector>
#include <boost/asio.hpp>


class Decoder
{
    public :
        Decoder();
        virtual ~Decoder(){};

        void initialize(boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<bounded_buffer<int> > buffer);
        //void initialize(boost::shared_ptr<queue<int> > buffer);

        void read_data();
        void read_queue();

        void start_gathering(boost::shared_ptr<int> event_counter);
        void stop_gathering();

        void print_final();
        int nReceived() { return n_received; }
        std::string thread() { return m_thread_id; }

    private :
        boost::shared_ptr<bounded_buffer<int> > m_buffer;
        boost::shared_ptr<boost::asio::io_service> m_io;
        boost::shared_ptr<queue<int> > m_queue;
        int n_received;
        int n_max_cap;
        std::vector<int> yep_vec;
        bool check_thread;
        std::string m_thread_id;
        boost::shared_ptr<int> m_event_counter;


}; // class

#endif
