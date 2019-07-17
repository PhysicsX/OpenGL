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

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glu.h>
#include <GL/glext.h>



#include "/home/ulas/soil/src/SOIL.h"


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

   glEnable(GL_DEPTH_TEST);
}

void DestroyContext(Display *d,GLXContext ctx)
{
    glXMakeCurrent(d, None, NULL);
    glXDestroyContext(d, ctx);
    ctx=NULL/*Nnullptr*/;
}
int i=XInitThreads();
void start(Display *d, Window w)
{
    static float glC2 = 0;
    static float angle2 = 0;


        auto win=XCreateSimpleWindow(d, w, 420,420,200,200,0,0,0);
        cout<<"mainwindow:"<<win<<endl;

        XMapWindow(d, win);
         GLXContext ctx =  CreateGLContext(d, win);

       while(1)
       {
             glC2 = glC2 + 0.001;
             angle2 = angle2 + 0.1;
             if(glC2 > 1)
             {
                 glC2 = 0;

             }
        glClearColor( glC2, glC2, glC2, 0.1f );
        glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        glRotatef(angle2,0,0.1,0);

        glBegin(GL_TRIANGLES);

       // glColor3f(0,0,1);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0,0.8,0);
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-0.8,-0.8,0);
        glColor3f(0.0,0.0,1.0);
        glVertex3f(0.8,-0.8,0);

        glEnd();

        glPopMatrix();

        glXSwapBuffers (d, win);

    }

       DestroyContext(d, ctx);
       XDestroyWindow(d,win);
}

GLuint loadBMP_custom(const char * imagepath)
{
     unsigned char header[54]; // Each BMP file begins by a 54-bytes header
     unsigned int dataPos;     // Position in the file where the actual data begins
     unsigned int width, height;
     unsigned int imageSize;   // = width*height*3
     // Actual RGB data
     unsigned char * data;

     FILE * file = fopen(imagepath, "rb");
     if (!file)
     {
         printf("Image could not be opened\n"); return 0;
     }
     if (fread(header, 1, 54, file) != 54){ // If not 54 bytes read : problem
         printf("Not a correct BMP file\n");
         return false;
     }

     if (header[0] != 'B' || header[1] != 'M'){
         printf("Not a correct BMP file\n");
         return 0;
     }

     // Read ints from the byte array
     dataPos = *(int*)&(header[0x0A]);
     imageSize = *(int*)&(header[0x22]);
     width = *(int*)&(header[0x12]);
     height = *(int*)&(header[0x16]);
     // Some BMP files are misformatted, guess missing information
     if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
     if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
     // Create a buffer
     data = new unsigned char[imageSize];
     // Read the actual data from the file into the buffer
     fread(data, 1, imageSize, file);
     //Everything is in memory now, the file can be closed
     fclose(file);

     GLuint textureID;
     glGenTextures(1, &textureID);
     glBindTexture(GL_TEXTURE_2D, textureID);

     // Give the image to OpenGL
     glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR,
GL_UNSIGNED_BYTE, data);

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

     // "Bind" the newly created texture : all future texture functions will modify this texture
     delete [] data;
     return textureID;
}





template<int>
void triangleSeris(GLfloat z,GLfloat r, GLfloat g, GLfloat b,GLfloat alpha)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_CONSTANT_ALPHA);

    static bool startFlag = 0;
    static GLuint plate=glGenLists(1);

    if(startFlag == 0)
    {
        startFlag = 1;

        glNewList(plate,GL_COMPILE_AND_EXECUTE);

        glBegin(GL_TRIANGLES);

        glColor4f(r,g,b,alpha);
        glVertex3f(-0.4,0.4,z);
        //glColor3f(0,0,0);
        glVertex3f(-0.4,-0.4,z);
        //glColor3f(0,0,0);
        glVertex3f(0.4,-0.4,z);

        glEnd();
        glEndList();

    }

    glCallList(plate);
    glDisable(GL_BLEND);


}

void rect(GLuint inT){

    glBindTexture(GL_TEXTURE_2D,inT);

    glBegin(GL_QUADS);
    //glColor3d(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-0.5,-0.5,0);
   // glColor3d(1,1,0);
    glTexCoord2f(1,0);
    glVertex3f(-0.5,0.5,0);
   // glColor3d(1,1,1);
    glTexCoord2f(1,1);
    glVertex3f(0.5,0.5,0);
   // glColor3d(0,1,1);
    glTexCoord2f(0,1);
    glVertex3f(0.5,-0.5,0);
    glEnd();


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
    auto win=XCreateSimpleWindow(display, parentmainw, xa.x,xa.y,xa.width-200,xa.height-200,0,0,0);
    cout<<"mainwindow:"<<win<<endl;

    XMapWindow(display, win);

    cout<<XSelectInput(display, win, KeyPressMask|ButtonPressMask)<<endl<<flush;

    Draw(display,parentmainw,0xff00,"Ulas D");
   // sleep(3);
   // thread t(start,display,parentmainw);

    GLXContext ctx =  CreateGLContext(display, win);

    static float glC = 0;
    static float angle = 0;

    glEnable(GL_TEXTURE_2D);

    GLuint inT = SOIL_load_OGL_texture("/home/ulas/opengl3/NeHe.bmp",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    //GLuint inT = loadBMP_custom("/home/ulas/opengl3/NeHe.bmp");
    //SOIL_load_OGL_texture("/home/ulas/opengl3/NeHe.bmp");


    GLuint colorTexture;

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 600,600, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);


    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 600,600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);


    GLuint fb;
    glGenFramebuffersEXT(1, &fb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTexture, 0/*mipmap level*/);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0/*mipmap level*/);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

   // glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);


    GLuint inT1 = loadBMP_custom("/home/ulas/opengl3/NeHe2.bmp");
    bool flagRun = 0;
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


    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glClearColor( 1, 0, 0, 0.1f );
    glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //glRotatef(angle,0,0.1,0);
    glTranslatef(1,0,0);
    rect(inT);
    glPopMatrix();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glClearColor( 0.2, 0.2, 0.2, 0.1f );
    glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glRotatef(angle,0,0.1,0);
    rect(colorTexture);
    glPopMatrix();

    glXSwapBuffers (display, win);

}
    //GLsizei in1[];
    glDeleteTextures(1,&inT);
    DestroyContext(display, ctx);
    XDestroyWindow(display,parentmainw);
    XCloseDisplay(display);
    //t.detach();
    return 0;

}
