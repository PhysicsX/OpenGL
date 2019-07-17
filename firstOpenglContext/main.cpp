#include <iostream>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <malloc.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <thread>

using namespace std;

void Draw(Display *display,Window w0,unsigned int col,const char *str)
{
    XGCValues values;
    GC gc=XCreateGC(display,w0,0,&values);
    XSetForeground(display,gc,col);

    XDrawString(display,w0,gc,10,10,str,strlen(str));

    XWindowAttributes xa={};
    XGetWindowAttributes(display,w0,&xa);

    XDrawRectangle(display,w0,gc,0,0,xa.width-1,xa.height-1);
    XFlush(display);

    XFreeGC(display,gc);
}

void DrawCloseCross(Display *display,Window w0)
{
    XGCValues values;
    GC gc=XCreateGC(display,w0,0,&values);

    XSetForeground(display,gc,0xffff);

    XWindowAttributes xa={};
    XGetWindowAttributes(display,w0,&xa);

    XDrawRectangle(display,w0,gc,xa.width-30,0,29,29);
    XDrawLine(display,w0,gc,xa.width-30,0,xa.width-1,30);
    XDrawLine(display,w0,gc,xa.width-30,30,xa.width-1,0);
    XFlush(display);

    XFreeGC(display,gc);
}

bool checkIfMouseIsOnCross(Display *display,Window w0,XEvent e)
{
    XWindowAttributes xa={};
    XGetWindowAttributes(display,w0,&xa);
    if( e.xbutton.x<xa.width && e.xbutton.x> xa.width-30)
        if( e.xbutton.y<30 && e.xbutton.y> 0)
            return true;
    return false;
}

int ErrorHandler (	    /* WARNING, this type not in Xlib spec */
                        Display*		/* display */,
                        XErrorEvent*	 error_event
                        )
{
    printf("Some X error\n");
    return 1;
}

int i=XInitThreads();
void start(Display *d, Window w)
{
    while(1)
    {
        sleep(1);
        auto win=XCreateSimpleWindow(d, w, 40,40,200,200,0,0,0);
        cout<<"mainwindow:"<<win<<endl;

        XMapWindow(d, win);

        sleep(1);
        Draw(d,win,0xff00,"main window");
        sleep(3);
        XDestroyWindow(d,win);
    }
}

unordered_set<Window> decoratedwindows;
unordered_map<Window,Window> mapWindowWithDecorated;

int main()
{
    Display *display = XOpenDisplay(NULL);  //get the display

    Window root = DefaultRootWindow(display);  /*get the root window*/
    Window parentmainw = XCreateSimpleWindow(display, root, 10,10,800,800,0,0,0);
    cout<<"created main window:"<<parentmainw<<" root:"<<root<<endl;
    XMapWindow(display,parentmainw);

    //cout<<XSelectInput(display, DefaultRootWindow(display), SubstructureNotifyMask|SubstructureRedirectMask|ExposureMask)<<endl;
    cout<<XSelectInput(display, parentmainw, SubstructureNotifyMask|SubstructureRedirectMask)<<endl<<flush;

    XSetErrorHandler(ErrorHandler);
    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);


    thread t(start,display,parentmainw);

    bool running = true;
    while(running){
        XEvent		event;
        while(XPending(display))
        {
            XNextEvent(display, &event);

            switch (event.type)
            {
            case CreateNotify:
                cout<<event.xcreatewindow.window<<endl;
                //if decorated window is not exist create one.
//                if(decoratedwindows.end()==decoratedwindows.find(event.xcreatewindow.window))
//                {
//                    cout<<"A window is created"<<endl<<flush;

//                    XWindowAttributes xa={};
//                    XGetWindowAttributes(display,event.xcreatewindow.window,&xa);
//                    cout<<"created window id "<<event.xcreatewindow.window<<endl<<flush;

//                    auto decorated=XCreateSimpleWindow(display, parentmainw, xa.x,xa.y,xa.width+60,xa.height+60,0,0,0);
//                    XMapWindow(display, decorated);

//                    XReparentWindow(display,event.xcreatewindow.window,decorated,30,30);

//                    Draw(display,decorated,0xff,"decorated");
//                    DrawCloseCross(display,decorated);

//                    XSelectInput(display, decorated, SubstructureNotifyMask|SubstructureRedirectMask);

//                    decoratedwindows.insert(decorated);
//                    mapWindowWithDecorated[event.xcreatewindow.window]=decorated;

//                    cout<<"created decorated window:"<<decorated<<endl<<flush;
//                }
//                else
//                {
//                    //this is my decorated window creation, ignore it
//                }
                break;
            case DestroyNotify:
                cout<<"want to destroy:"<<event.xdestroywindow.window<<endl<<flush;

                if(decoratedwindows.end()!=decoratedwindows.find(event.xdestroywindow.window))
                {
                    //dont do anything
                    decoratedwindows.erase(decoratedwindows.find(event.xdestroywindow.window));

                    //find its main window to remove its mapping
                    Window mainwindow;
                    for(const auto &p:mapWindowWithDecorated)
                    {
                        if(p.second==event.xdestroywindow.window)
                        {
                           mainwindow=p.first;
                           break;
                        }
                    }
                    mapWindowWithDecorated.erase(mainwindow);
                }

                XDestroyWindow(display,mapWindowWithDecorated[event.xdestroywindow.window]);

                break;

            case ClientMessage:
            if (event.xclient.data.l[0] == wmDeleteMessage)
                running = false;
            break;
            }
        }
    }

    XDestroyWindow(display,parentmainw);
    XCloseDisplay(display);
    t.detach();
    return 0;
}
