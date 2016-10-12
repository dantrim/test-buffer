#ifndef DECODER_HH
#define DECODER_HH

#include "bounded_buffer.hh"
#include "queue.hh"
#include <vector>


class Decoder
{
    public :
        Decoder();
        virtual ~Decoder(){};

        void initialize(boost::shared_ptr<bounded_buffer<int> > buffer);
        //void initialize(boost::shared_ptr<queue<int> > buffer);

        void read_data();
        void read_queue();

        void print_final();
        int nReceived() { return n_received; }

    private :
        boost::shared_ptr<bounded_buffer<int> > m_buffer;
        boost::shared_ptr<queue<int> > m_queue;
        int n_received;
        int n_max_cap;
        std::vector<int> yep_vec;


}; // class

#endif
