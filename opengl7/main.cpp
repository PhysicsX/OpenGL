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
#include <math.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glu.h>
#include <GL/glext.h>


using namespace std;

GLXContext CreateGLContext(Display *d,Window w)
{
    static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,GLX_DEPTH_SIZE,16,None };
    XVisualInfo *vi = glXChooseVisual(d, DefaultScreen(d),attributeList);
    GLXContext ctx = glXCreateContext(d, vi, 0, GL_TRUE);
    glXMakeCurrent (d, w, ctx);

   glEnable(GL_DEPTH_TEST);
}

void DestroyContext(Display *d,GLXContext ctx)
{
    glXMakeCurrent(d, None, NULL);
    glXDestroyContext(d, ctx);
    ctx=NULL/*Nnullptr*/;
}

template<int i>
void RenderSquareTextured(float offset)
{
//glPushMatrix();
static __thread float f=0;f=f-1.01*offset*5;
//glRotatef(f,1,1,1);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3f( 1.0f, i, 0.0f );glTexCoord2f(0.0f, 0.0f); glVertex3f(offset-1.0f/2, -1.0f/2, 0);
    glColor3f( 1.0f, i, 0.0f );glTexCoord2f(1.0f, 0.0f); glVertex3f(offset+1.0f/2, -1.0f/2, 0);
    glColor3f( 1.0f, i, 0 );glTexCoord2f(1.0f, 1.0f); glVertex3f(offset+1.0f/2, 1.0f/2, 0);
    glColor3f( 1.0f, i, 0.0f );glTexCoord2f(0.0f, 1.0f); glVertex3f(offset-1.0f/2, 1.0f/2, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

}
void startFog()
{
    GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR }; // Storage For Three Types Of Fog
    GLuint fogfilter= 1; // Which Fog To Use
    GLfloat fogColor[4]= {0.0f, .0f, 1.0f, 0.0f};

    glFogi(GL_FOG_MODE, fogMode[fogfilter]); // Fog Mode
    glFogfv(GL_FOG_COLOR, fogColor); // Set Fog Color
    glFogf(GL_FOG_DENSITY, 2.0f); // How Dense Will The Fog Be
    glHint(GL_FOG_HINT, GL_DONT_CARE); // Fog Hint Value
    glFogf(GL_FOG_START, 0.1f); // Fog Start Depth
    glFogf(GL_FOG_END, 0.3f); // Fog End Depth
    glEnable(GL_FOG);
}
void stopFog()
{
glDisable(GL_FOG);
}


int main()
{

    Display *display = XOpenDisplay(NULL);  //get the display

    Window root = DefaultRootWindow(display);  /*get the root window*/
    Window parentmainw = XCreateSimpleWindow(display, root, 10,10,800,800,0,0,0);
    cout<<"created main window:"<<parentmainw<<" root:"<<root<<endl;
    XMapWindow(display,parentmainw);
    XSelectInput(display, parentmainw, KeyPressMask|ButtonPressMask);

  //  XSetErrorHandler(ErrorHandler);
    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);

    XWindowAttributes xa={};
    XGetWindowAttributes(display,parentmainw,&xa);

    //sleep(1);
    auto win=XCreateSimpleWindow(display, parentmainw, xa.x,xa.y,xa.width-200,xa.height-200,0,0,0);
    cout<<"mainwindow:"<<win<<endl;

    XMapWindow(display, win);

    cout<<XSelectInput(display, win, KeyPressMask|ButtonPressMask)<<endl<<flush;

    XMapWindow(display, win);
    GLXContext ctx =  CreateGLContext(display, win);
    glEnable(GL_TEXTURE_2D);
    bool running = true;

    while(running)
    {

        XEvent		event;
        while(XPending(display))
        {
            XNextEvent(display, &event);

            switch (event.type)
            {
            case ButtonPress:
            {
                XButtonEvent *xbuttonevent=(XButtonEvent*)&event;


               // std::cout<<xbuttonevent->button<<std::endl<<flush;
                break;
            }
            case KeyPress:
            {
                XKeyEvent *xkey = (XKeyEvent*)&event;
               //std::cout<<xkey->keycode<<std::endl<<flush;
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

            glClearColor( 0.2, 0.2, 0.2, 0.1f );
            glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

            glTranslatef(0.0, 0.0, 0.0001);
            startFog();
            RenderSquareTextured<0>(0.0);
            stopFog();

//            glBegin(GL_QUADS);

//            glColor3d(1,1,0);

//            glVertex3f(-0.5,-0.5,0);
//           // glColor3d(1,1,0);

//            glVertex3f(-0.5,0.5,0);
//           // glColor3d(1,1,1);

//            glVertex3f(0.5,0.5,0);
//           // glColor3d(0,1,1);


//            glEnd();

            glXSwapBuffers (display, win);

    }

    DestroyContext(display, ctx);
    XDestroyWindow(display,win);
    XCloseDisplay(display);
    return 0;
}
