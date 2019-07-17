#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

    XImage *g_xImage = NULL;
void saveToMem(Display *d,Window w)
{
    if(g_xImage)
        XFree(g_xImage);

    g_xImage = XGetImage(d,w,0,0,600,600,AllPlanes,XYPixmap);
    //XGetPixel(g_xImage,10,10);
}
void putfromMem(Display *d,Window w)
{
    XGCValues values;
    GC gc = XCreateGC(d, w, 0, &values);
    XPutImage(d,w,gc,g_xImage,0,0,0,0,600,500);
    XFreeGC(d, gc);
}


int main()
{
    cout << "Hello World!" << endl;
    Display *display = XOpenDisplay(NULL);

    Window root = DefaultRootWindow(display);
    Window parentmainw = XCreateSimpleWindow(display,root,10,10,800,800,0,0,0);

    Window childWindow = XCreateSimpleWindow(display,parentmainw,10,10,790,790,0,0,0);

    XStoreName(display, parentmainw, "parent window");
    XMapWindow(display,parentmainw);
    XSelectInput(display, parentmainw, KeyPressMask|ButtonPressMask);

    XStoreName(display, childWindow, "child window");
    XMapWindow(display,childWindow);
    XSelectInput(display, childWindow, KeyPressMask|ButtonPressMask);

    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);

    XEvent event;
    char *text = "hello";
    bool running = true;




    while(running)
    {

        XGCValues valuesC;
        GC gcC = XCreateGC(display, childWindow, 0, &valuesC);
        XSetForeground(display, gcC, 0xbb);
        XDrawString(display, childWindow, gcC,11,10, "hello child", 12);

        XGCValues values;
        GC gc = XCreateGC(display, parentmainw, 0, &values);
        XSetForeground(display, gc, 0xff);
        XDrawString(display, parentmainw, gc,11,400, "hello", 6);


        while(XPending(display))
        {

            XEvent event;
            XNextEvent(display, &event);


            switch (event.type) {
            case ButtonPress:
            {
                    XButtonEvent *xbuttonevent=(XButtonEvent*)&event;
                    if(1==xbuttonevent->button)
                    {
                       XDrawRectangle(display,childWindow,gcC,xbuttonevent->x,xbuttonevent->y,50,50);
                  //     g_xImage = XGetImage(display,childWindow,0,0,600,600,AllPlanes,XYPixmap);
                 //      std::cout<<XGetPixel(g_xImage,xbuttonevent->x, xbuttonevent->y);

                    }

                    if(3==xbuttonevent->button)
                        XDrawArc(display, childWindow, gcC, xbuttonevent->x, xbuttonevent->y, 30, 30, 0, 360*64);


                    std::cout<<xbuttonevent->button<<std::endl;
                break;
            }
            case KeyPress:
            {
                   // running = false;
                XKeyEvent *keyEvent = (XKeyEvent*)&event;
                    if(39==keyEvent->keycode)
                    {
                        saveToMem(display, childWindow);
                    }

                    if(46==keyEvent->keycode)
                    {
                        putfromMem(display, childWindow);
                    }


                std::cout<<keyEvent->keycode<<std::endl;

                break;
            }
            case ClientMessage:
                 if (event.xclient.data.l[0] == wmDeleteMessage)
                               running = false;
                break;

            }
        }

        XFreeGC(display, gcC);
        XFreeGC(display, gc);
    }




    XDestroyWindow(display, parentmainw);
    XCloseDisplay(display);

    return 0;
}
