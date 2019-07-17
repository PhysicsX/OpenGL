#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>
#include <string.h>
#include <thread>
#include <unistd.h>

using namespace std;
void xdrwstring(Display *display, Window window,int x, int y, char *arr)
{

    XGCValues valuesC;
    GC gcC = XCreateGC(display, window, 0, &valuesC);
    XSetForeground(display, gcC, 0xbb);
    XDrawString(display, window, gcC,x,y, arr, strlen(arr)+1);
    XFreeGC(display, gcC);

}
Window childWindow;
void mythread(Display *display, Window window)
{

    childWindow = XCreateSimpleWindow(display,window,10,10,790,790,0,0,0);
    XStoreName(display, window, "child window");
    XMapWindow(display,childWindow);
    XSelectInput(display, window, KeyPressMask|ButtonPressMask);

    sleep(2);
    xdrwstring(display, childWindow, 1, 10,"I am child");

}

int main()
{


    int i=XInitThreads();

    Display* display = XOpenDisplay(NULL);

    Window root = DefaultRootWindow(display);

    Window parentmainw = XCreateSimpleWindow(display,root,50,50,800,800,0,0,0);
    XStoreName(display, parentmainw, "parent window");
    XMapWindow(display,parentmainw);


    XSelectInput(display, parentmainw, SubstructureNotifyMask|SubstructureRedirectMask);

    thread t(mythread, display, parentmainw);
    t.detach();

    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);


    bool running = true;
    while(running)
    {
        xdrwstring(display, parentmainw, 1, 10,"I am parent");



        while(XPending(display))
        {

            XEvent event;
            XNextEvent(display, &event);
            switch (event.type) {
                case CreateNotify:
                {
                    std::cout<<"Print Sth";
                    std::cout<<flush;
                }
                case ClientMessage:
                if (event.xclient.data.l[0] == wmDeleteMessage)
                    running = false;
                break;

            }

        }


    }

    XDestroyWindow(display, parentmainw);
    XCloseDisplay(display);
    return 0;
}
