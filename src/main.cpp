#include <iostream>
#include "container.h"

using namespace std;

Container<int> c;

int main()
{
    try
    {
        c["A1"]=121;
        c.set("A1-A1",322);
        c.set("A2-A1",322);
        c.set("A2-A2",322);
        c.set("A2",321);
        c.set("A3",321);
        c.set("A4",321);
        c.set("A9",340);
        std::cout << c << std::endl;

    }  catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
