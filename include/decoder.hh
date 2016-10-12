#ifndef DECODER_HH
#define DECODER_HH

#include "bounded_buffer.hh"


class Decoder
{
    public :
        Decoder();
        virtual ~Decoder(){};

        void initialize(boost::shared_ptr<bounded_buffer<int> > buffer);

        void read_data();


    private :
        boost::shared_ptr<bounded_buffer<int> > m_buffer;
        int n_received;


}; // class

#endif
