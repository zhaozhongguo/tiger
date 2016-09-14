#include <iostream>
using namespace std;

int main()
{
    char buf[50] = {0};

    void* p = buf;
    void* p2 = buf + 2;
    cout<<((char*)p2 - (char*)p)<<endl;
    uint32_t dd = 43;

    return 0;
}