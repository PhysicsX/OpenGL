#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <algorithm>
#include <vector>

int i=XInitThreads();

using namespace std;
void xdrwstring(Display *display, Window window,int x, int y, char *arr)
{

     XGCValues valuesC;
     GC gcC = XCreateGC(display, window, 0, &valuesC);
     XSetForeground(display, gcC, 0xbb);
     XDrawString(display, window, gcC,x,y, arr, strlen(arr)+1);

     XFreeGC(display, gcC);

}
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

void xdrwRect(Display *display, Window window,int x, int y)
{

     XGCValues valuesC;
     GC gcC = XCreateGC(display, window, 0, &valuesC);
     XSetForeground(display, gcC, 0xFF);
     XDrawRectangle(display,window,gcC,x,y,400,400);
     XFlush(display);
     XFreeGC(display, gcC);

}
Window childWindow;
void mythread(Display *display, Window window)
{
    while(true)
    {
     childWindow =
     XCreateSimpleWindow(display,window,200,200,400,400,0,0,0);
     sleep(1);
     XStoreName(display, window, "child window");
     XMapWindow(display,childWindow);
     sleep(1);
     Draw(display, childWindow,0xff,"child");


     sleep(2);
     XDestroyWindow(display,childWindow);
    }
}

int main()
{

     Display* display = XOpenDisplay(NULL);

     Window root = DefaultRootWindow(display);

     Window parentmainw = XCreateSimpleWindow(display,root,50,50,800,800,0,0,0);
     XStoreName(display, parentmainw, "parent window");
     XMapWindow(display,parentmainw);

     XSelectInput(display, parentmainw, SubstructureNotifyMask|SubstructureRedirectMask);

     thread t(mythread, display, parentmainw);
   //  t.detach();

     static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
     XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);

    int array [5];
    int j;
    int a;
    //Window win = NULL;
    vector<Window> win;
Window decorator;
     bool running = true;
     while(running)
     {
        // xdrwstring(display, parentmainw, 1, 10,"I am parent");
        // xdrwRect(display,parentmainw,10,10);
        // Draw(display, childWindow,0xff,"child");


         while(XPending(display))
         {

             XEvent event;
             XNextEvent(display, &event);



             switch (event.type) {
                 case CreateNotify:
                 {

                     std::cout<<"Print Sth";
                     std::cout<<flush;
                     std::cout<<event.xcreatewindow.window<<std::endl;
                     std::vector<Window>::iterator it = std::find(win.begin(),win.end(),event.xcreatewindow.window);

                     if(it == win.end())
                     {
                         XWindowAttributes xa={};
                         XGetWindowAttributes(display,event.xcreatewindow.window,&xa);

                         decorator = XCreateSimpleWindow(display,parentmainw,xa.x-30,xa.y-30,xa.width+60,xa.height+60,0,0,0);
                         win.push_back(decorator);

                         XMapWindow(display,decorator);
                         XSelectInput(display, decorator, SubstructureNotifyMask|SubstructureRedirectMask);
                         sleep(1);

                         XReparentWindow(display,event.xcreatewindow.window,decorator,30,30);
                        Draw(display,decorator,0xff00,"decorator");
                     }

                 // XDrawRectangle(display,decorator,gC,0,0,50,50);
//                    XWindowAttributes xa={};
// XGetWindowAttributes(display,event.xcreatewindow.window,&xa);

                    break;
                 }
                 case DestroyNotify:
                 {

                 std::vector<Window>::iterator it = std::find(win.begin(),win.end(),event.xcreatewindow.window);
                 if(it == win.end())
                 {
                    XDestroyWindow(display,decorator);
                    XFlush(display);
                 }

                 break;
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
      t.detach();
     return 0;
}

