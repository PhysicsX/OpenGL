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

#include <GL/gl.h>
#include <GL/glx.h>

using namespace std;

static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,GLX_DEPTH_SIZE,16,None };


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

GLXContext createContext(Display *d, Window w)
{
    XVisualInfo *vi = glXChooseVisual(d, DefaultScreen(d),attributeList);
    GLXContext ctx = glXCreateContext(d, vi, 0, GL_TRUE);
    glXMakeCurrent (d, w, ctx);

    return ctx;
}

GLXContext CreateGLContext(Display *d,Window w)
{
    static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,GLX_DEPTH_SIZE,16,None };
    XVisualInfo *vi = glXChooseVisual(d, DefaultScreen(d),attributeList);
    GLXContext ctx = glXCreateContext(d, vi, 0, GL_TRUE);
    glXMakeCurrent (d, w, ctx);
    fflush(stdout);
    fflush(stderr);
}

void DestroyContext(Display *d,GLXContext ctx)
{
    glXMakeCurrent(d, None, NULL);
    glXDestroyContext(d, ctx);
    ctx=NULL/*Nnullptr*/;
}


int main()
{

    cout << "Hello World!" << endl;

    Display *display = XOpenDisplay(NULL);  //get the display

    Window root = DefaultRootWindow(display);  /*get the root window*/
    Window parentmainw = XCreateSimpleWindow(display, root, 10,10,800,800,0,0,0);
    cout<<"created main window:"<<parentmainw<<" root:"<<root<<endl;
    XMapWindow(display,parentmainw);

    cout<<XSelectInput(display, parentmainw, KeyPressMask|ButtonPressMask)<<endl<<flush;

    XSetErrorHandler(ErrorHandler);
    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);

    XWindowAttributes xa={};
    XGetWindowAttributes(display,parentmainw,&xa);

    //sleep(1);
    auto win=XCreateSimpleWindow(display, parentmainw, xa.x,xa.y,xa.width,xa.height,0,0,0);
    cout<<"mainwindow:"<<win<<endl;

    XMapWindow(display, win);

    cout<<XSelectInput(display, win, KeyPressMask|ButtonPressMask)<<endl<<flush;

    Draw(display,parentmainw,0xff00,"Ulas D");
   // sleep(3);


    GLXContext ctx =  CreateGLContext(display, win);

    static float glC = 0;
    static float angle = 0;

    bool running = true;
    while(running)
    {
        glC = glC + 0.001;
        angle = angle + 1;
        if(glC > 1)
        {
            glC = 0;

        }
        XEvent		event;

    while(XPending(display))
    {
        XNextEvent(display, &event);

        switch (event.type)
        {
        case ButtonPress:
        {
            XButtonEvent *xbuttonevent=(XButtonEvent*)&event;


            std::cout<<xbuttonevent->button<<std::endl<<flush;
            break;
        }
        case KeyPress:
        {
            XKeyEvent *xkey = (XKeyEvent*)&event;
            std::cout<<xkey->keycode<<std::endl<<flush;
            running = false;
            break;
        }
        case CreateNotify:


            break;
        case DestroyNotify:



            break;

        case ClientMessage:
        if (event.xclient.data.l[0] == wmDeleteMessage)
            running = false;
        break;
        }
    }

    glClearColor( glC, glC, glC, 0.1f );
    glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(angle,0,0.1,0);

    glBegin(GL_TRIANGLES);

    glColor3f(1,0.05,0.1);
    glVertex3f(0,0.8,0);
    glVertex3f(-0.8,-0.8,0);
    glVertex3f(0.8,-0.8,0);

    glEnd();

    glPopMatrix();

    glXSwapBuffers (display, win);

}

    DestroyContext(display, ctx);
    XDestroyWindow(display,parentmainw);
    XCloseDisplay(display);

    return 0;

}
