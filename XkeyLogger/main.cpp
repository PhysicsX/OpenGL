#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


using namespace std;

int main()
{
    //cout << "Hello World!" << endl;
    Display* display = XOpenDisplay(NULL);
    char arr[32];

    while(true)
    {
        XQueryKeymap(display, arr);
        for(int i=0; i<32; i++)
        {
            std::cout<<arr[i];
        }
    }
    return 0;
}
//#include <X11/Xutil.h>


//using namespace std;

//int main()
//{
//    //cout << "Hello World!" << endl;
//    Display* display = XOpenDisplay(NULL);


//    while(true)
//    {
//        char arr[32];
//        XQueryKeymap(display, arr);
//        for(char c:arr)
//        {
//            std::cout<<c;
//        }
//        cout<<endl;
//    }
//    return 0;
//}
