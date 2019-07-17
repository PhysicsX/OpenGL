#include <iostream>
#include <X11/Xlib.h>


using namespace std;




int main()
{
    cout << "Hello World!" << endl;
    Display *display = XOpenDisplay(NULL);

    Window root = DefaultRootWindow(display);
    Window parentmainw = XCreateSimpleWindow(display,root,10,10,800,800,0,0,0);

    Window childWindow = XCreateSimpleWindow(display,parentmainw,10,10,120,120,0,0,0);

    XStoreName(display, parentmainw, "parent window");
    XMapWindow(display,parentmainw);
    XSelectInput(display, parentmainw, KeyPressMask|ButtonPressMask);

    XStoreName(display, childWindow, "child window");
    XMapWindow(display,childWindow);
    XSelectInput(display, childWindow, KeyPressMask|ButtonPressMask);

    XEvent event;
    char *text = "hello";
    bool running = true;
    while(running)
    {

        XGCValues valuesC;
        GC gcC = XCreateGC(display, childWindow, 0, &valuesC);
        XSetForeground(display, gcC, 0xaa);
        XDrawString(display, childWindow, gcC,11,10, "hello child", 12);

        XGCValues values;
        GC gc = XCreateGC(display, parentmainw, 0, &values);
        XSetForeground(display, gc, 0xff);
        XDrawString(display, parentmainw, gc,11,400, "hello", 6);

        XFreeGC(display, gcC);
        XFreeGC(display, gc);

        while(XPending(display))
        {


            XEvent event;
            XNextEvent(display, &event);
            switch (event.type) {
            case KeyPress:
                    running = false;
                break;
            }
        }
    }

    XDestroyWindow(display, parentmainw);
    XCloseDisplay(display);

    return 0;
}
