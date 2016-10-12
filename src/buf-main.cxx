#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;

//boost
#include <boost/circular_buffer.hpp>

int main(int argc, char* argv[]) {

    cout << "test-buffer" << endl; 

    boost::circular_buffer<int> cb(3);

    for(int i = 0; i < 3; i++)
        cb.push_back(i);

    for(int i = 0; i < (int)cb.size(); i++)
        cout << "1) cb[" << i << "] : " << cb[i] << endl;

    for(int i = 5; i < 6; i++)
        cb.push_back(i);
    cout << endl;

    for(int i = 0; i < (int)cb.size(); i++)
        cout << "2) cb[" << i << "] : " << cb[i] << endl;

    cout << endl;
    cout << "pop-back" << endl;
    cb.pop_back();
    for(int i = 0; i < (int)cb.size(); i++)
        cout << "pop-back) cb[" << i << "] : " << cb[i] << endl;

    cout << endl;
    cout << "push-front" << endl;
    cb.push_front(9);
    for(int i = 0; i < (int)cb.size(); i++)
        cout << "pop-front) cb[" << i << "] : " << cb[i] << endl;
    
    



  return 0;
}
